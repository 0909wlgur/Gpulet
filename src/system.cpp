#include "system.hpp"

System::System(const std::vector<std::string>& model_names, 
            const std::vector<double>& model_slos, const std::vector<double>& model_rates) 
            : gpu_num(0) {
    
    // 0. Initialize the number of GPU and GPUlet
    assert(model_names.size() == model_rates.size() && model_names.size() == model_slos.size());
    
    int model_len = model_names.size();

    // 1. Make model_list of the system
    for(int i = 0; i < model_len; i++) {
        model_inference_info* model = new model_inference_info;
        
        model->model_id = i;
        model->model_name = model_names[i];
        model->execution_rate = model_rates[i];
        model->slo = model_slos[i] / 1000;

        model_list.push_back(model);
    }

    // 2. Make gpu_list of the system.
    add_gpu();

    // 3. Execute the elastic partitioning
    elastricPartitioning();

    std::cout << "System generation is complete.\n" << std::endl;
    std::cout << "The number of needed GPU is " << gpu_num << std::endl << std::endl;
}

void System::elastricPartitioning() {
    
    // 1. Sort every model by rate * SLO in ascending order.
    auto model_list = this->model_list;
    std::sort(model_list.begin(), model_list.end(), 
        [](const model_inference_info* a, const model_inference_info* b) {
            return a->execution_rate * a->slo < b->execution_rate * b->slo;
    });

    // 2. Get a gpulet for every inference model
    int model_length = model_list.size();
    for (int m = 0; m < model_length; m++) {
        auto model = model_list[m];
        double rate = model->execution_rate;
        double decrease_rate;

        std::cout << m << "th model started" << std::endl;
        std::cout << "model name: " << model->model_name << std::endl;
        std::cout << "slo: " << model->slo << std::endl << std::endl;

        while (rate > 0) {
            int p_eff = get_eff_partition_rate(model->model_name);
            int p_req = get_req_partition_rate(model->model_name, rate);
            int p_ideal = std::min(p_eff, p_req);

#ifdef DEBUG_PARTITION
            std::cout << "left rate: " << rate << std::endl;
            std::cout << "model eff partition: " << p_eff << std::endl;
            std::cout << "model req partition: " << p_req << std::endl;
            std::cout << "model ideal partition: " << p_ideal << "\n" << std::endl;
#endif
            int model_id = model->model_id;
            Gpulet gpulet = findBestFit(model_id, p_ideal);
            // std::cout << "come back to elastic partitioning from findBestFit" << std::endl;
            gpulet.fixGpulet(*model);
            int gpu_id = gpulet.getGPUId();
            
            if (gpu_id == -1) {
                std::cerr << "Function findBestFit is failed" << std::endl;
                break;
            } else {
                // Apply gpulet to system
                // std::cout << "elasticPartitioning -> Apply gpulet to system Part" << std::endl;
                
                auto gpu = gpu_list[gpu_id];
                gpu->setGpulet(gpulet);
                decrease_rate = gpulet.getRate();
                // std::cout << "decrease_rate: " << decrease_rate << std::endl;
            }
            rate = rate - decrease_rate;
            // std::cout << "rate: " << rate << std::endl;
            // gpu_list[gpu_id]->printGPUInfo();
        }
        std::cout << "------------------------------------------" << std::endl;
    }

}

Gpulet System::findBestFit(int model_id, int p_ideal) {

    // 0. Make available gpulet list.
    std::vector<std::shared_ptr<Gpulet>> available_gpulet_list = make_available_gpulet_list();

    if (available_gpulet_list.size() == 0) {
        add_gpu();
        available_gpulet_list = make_available_gpulet_list();
    }
    
    // 1. Sort every model by partition size in ascending order.
    std::sort(available_gpulet_list.begin(), available_gpulet_list.end(), 
    [](const std::shared_ptr<Gpulet>& a, const std::shared_ptr<Gpulet>& b) {
        return a->getPartition() < b->getPartition();
    });

    // 2. Find fitting gpulet. 
    auto model = model_list[model_id];
    int gpulet_cnt = available_gpulet_list.size();    

    for(int i = 0; i < gpulet_cnt; i++) {
        auto gpulet = available_gpulet_list[i];
        std::cout << "Checking GPU ID: "<< gpulet->getGPUId() << std::endl;

        int available_resource = gpulet->getPartition();
        
        if (available_resource >= p_ideal) {
            std::vector<model_info> model_info_vector;
            
            model_info model1;
            model1.model_name = model->model_name;
            model1.partition = p_ideal;
            model_info_vector.push_back(model1);

            if (available_resource == 100) {
                // Split by p_ideal and allocate the model
                gpulet->fixPartition(p_ideal);
                gpulet->fixGpulet(*model);

                // 어차피 1개만 넣는 거라 상관 없는 상황
            } else {
                // Allocate to the gpu->second

#ifdef DEBUG_PARTITION
                if (p_ideal != available_resource)
                    std::cout << "partition has been changed to " << available_resource << std::endl << std::endl;
                else   
                    std::cout << "The space and space needed are exactly same" << std::endl << std::endl;
#endif
                gpulet->fixPartition(available_resource);
                model1.partition = available_resource;
                gpulet->fixGpulet(*model);
                
                int gpu_id = gpulet->getGPUId();
                auto inter_gpulet = gpu_list[gpu_id]->getFirst();

                // 2개 넣는 상황
                model_info inter_model;
                inter_model.model_name = inter_gpulet->getModelInfo().model_name;
                inter_model.partition = inter_gpulet->getPartition();
                inter_model.batch = inter_gpulet->getBatch();
                model_info_vector.push_back(inter_model);
            }
            
            // Set batch size
            gpulet->setBatch(model_info_vector);

            model1.batch = gpulet->getBatch();
            gpulet->setRate(model1);
            gpulet->printGpuletInfo();

            return *gpulet;
        }
    }
    
    // 존재하는 GPU로 불가능한 상황
    add_gpu();
    int new_gpu_idx = getGPUnum() - 1;
    std::vector<model_info> model_info_vector;
            
    model_info model1;
    model1.model_name = model->model_name;
    model1.partition = p_ideal;
    model_info_vector.push_back(model1);

    auto gpulet = std::make_shared<Gpulet>(100, new_gpu_idx);
    gpulet->fixPartition(p_ideal);
    gpulet->fixGpulet(*model);

    gpulet->setBatch(model_info_vector);

    model1.batch = gpulet->getBatch();
    gpulet->setRate(model1);
    gpulet->printGpuletInfo();

    return *gpulet;
}

std::vector<std::shared_ptr<Gpulet>> System::make_available_gpulet_list() {
    std::vector<std::shared_ptr<Gpulet>> available_gpulet_list;
    
    for(int g = 0; g < gpu_num; g++) {
        // std::cout<< "GPU: " << g << std::endl;
        auto gpu = gpu_list[g];
        int available_resource = gpu->getAvailableResource();
        
        if (available_resource == 100) {
            // When GPU has (100, none)
            // std::cout<<"When available_resource = 100" << std::endl;
            
            auto new_gpulet = std::make_shared<Gpulet>(available_resource, g);
            // std::cout << "After making new_gpulet" << std::endl;

            available_gpulet_list.push_back(new_gpulet);
            // std::cout << "After push back new_gpulet" << std::endl;

            std::cout<< "For gpulet list check GPU: " << g << std::endl;
        } else if (available_resource != 100 && available_resource != 0) {
            // When GPU has (100 - p_ideal, p_ideal)
            // std::cout << "When available_resource != 100" << std::endl;

            auto new_gpulet = std::make_shared<Gpulet>(available_resource, g);
            available_gpulet_list.push_back(new_gpulet);
        }
    }
    // std::cout << "Before return " << std::endl;
    return available_gpulet_list;
}

double System::get_remain_rate(int order) {
    return this->model_list[order]->execution_rate;
}

int System::get_req_partition_rate(const std::string& model_name, double rate) {
    model_info model;
    model.model_name = model_name;
    model.batch = 16;

    const std::vector<int> partition = {20, 40, 50, 60, 80, 100};

    for(int i = 0; i < 6; i++) {
        model.partition = partition[i];
        double throughput = getThroughput(model);

        if (throughput >= rate) {
            return partition[i];
        }
    }
    return 100;
}

int System::get_eff_partition_rate(const std::string& model_name) {
    model_info model;
    model.model_name = model_name;
    model.batch = 16;

    const std::vector<int> partition = {0, 20, 40, 50, 60, 80, 100};

    double max_value_slope = 0;
    int max_value_idx = 0;

    for (int i = 0; i < 6; i++) {
        int p2 = partition[i + 1];
        int p1 = partition[i];

        model.partition = p2;

        double t2 = getThroughput(model);
        double t1;
        if (p1 == 0)
            t1 = 0;
        else {
            model.partition = p1;
            t1 = getThroughput(model);
        }
            

        double slope = (t2 - t1) / (p2 - p1);
#ifdef DEBUG_SLOPE
        std::cout << "slope of " << i << ": " << slope << std::endl;
#endif
        if (slope > max_value_slope) {
            max_value_idx = i + 1;
            max_value_slope = slope;
        }
    }
    return partition[max_value_idx];
}

void System::add_gpu() {
    auto gpu = new Gpu(gpu_num);
    gpu_list.push_back(gpu);
    gpu_num++;
}

int System::getGPUnum() {
    return gpu_num;
}

void System::printModelList() {
    int length = model_list.size();

    for (int i = 0; i < length; i++) {
        std::cout << "model_id: " << model_list[i]->model_id << std::endl;
        std::cout << "model_name: " << model_list[i]->model_name << std::endl;
        std::cout << "slo: " << model_list[i]->slo << std::endl;
        std::cout << "rate: " << model_list[i]->execution_rate << std::endl;
        std::cout << std::endl;
    }
}

void System::printPartitioning() {
    for (int g = 0; g < gpu_num; g++) {
        gpu_list[g]->printGPUInfo();
    }
}