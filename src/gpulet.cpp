#include "gpulet.hpp"

// #define DEBUG_SETBATCH

double is_remain_rate(const model_inference_info& model) {
    return model.execution_rate;
}

Gpulet::Gpulet(int available_resource, int gpuId_)
    : resource(available_resource) {
    model = new model_inference_info;
    gpuId = gpuId_;
}

int Gpulet::getPartition() {
    return resource;
}

Gpulet* Gpulet::splitAndAllocate(model_inference_info model, int p_ideal) {
    // confirm the partition and model_inference_info of the gpulet.
    fixGpulet(model);
                
    fixPartition(p_ideal);

    // push back the (100 - p_ideal) size partition
    int new_partition = 100 - p_ideal;
    auto gpulet = new Gpulet(new_partition);

    return gpulet;
}

void Gpulet::fixGpulet(model_inference_info model_) {
    // std::cout << "gpulet->fixGpulet enter" << std::endl;
    model->model_id = model_.model_id;
    model->model_name = model_.model_name;
    model->slo = model_.slo;
}

void Gpulet::fixPartition(int p_ideal) {
    resource = p_ideal;
}

void Gpulet::fixBatch(int batch) {
    this->batch = batch;
}

void Gpulet::fixRate(double rate) {
    model->execution_rate = rate;
}

int Gpulet::getGPUId() {
    return gpuId;
}

int Gpulet::getBatch() {
    return batch;
}

double Gpulet::getRate() {
    return model->execution_rate;
}

model_inference_info Gpulet::getModelInfo() {
    return *model;
}

void Gpulet::setBatch(std::vector<model_info> model_info_vector) {
    std::string model_name = model->model_name;

    std::vector<int> batches = {1, 2, 4, 8, 16, 32, 64};
    int i = 0;

    for (i = 0; i < batches.size(); i++) {
        int batch = batches[i];

        model_info model_;
        model_.model_name = model_name;
        model_.partition = resource;
        model_.batch = batch;

        double latency = getLatency(model_);

        double interference = 0;

        if (model_info_vector.size() == 2) {
            model_info model = model_info_vector[0];
            model.batch = batch;
            model_info inter_model = model_info_vector[1];
            
            mem_entry model_mem = getMemEntry(model);
            mem_entry inter_model_mem = getMemEntry(inter_model);

            double* interference_coeff = new double[5];

            *interference_coeff = 0.000291343;
            *(interference_coeff + 1) = 0.000364121;
            *(interference_coeff + 2) = 0.000101413;
            *(interference_coeff + 3) = 8.24321e-05;
            *(interference_coeff + 4) = 0.00309856;
            // get_interfere_coefficient(interference_coeff);

            interference = interference_coeff[0] * model_mem.L2_util + interference_coeff[1] * model_mem.Mem_util +
                    interference_coeff[2] * inter_model_mem.L2_util + interference_coeff[3] * inter_model_mem.Mem_util + 
                    interference_coeff[4];
            delete[] interference_coeff;
        }

#ifdef DEBUG_SETBATCH
        std::cout << "try batch: " << batch << std::endl;
        std::cout << "latency: " << latency * 1000 << std::endl;
        std::cout << "interfernce: " << interference * 1000 << "\n" << std::endl;
#endif

        // interference 들어가야 함.
        if (latency + interference > model->slo)
            break;
    }

    if (i == 0) {
        this->batch = 1;
    } else {
        this->batch = batches[i - 1];
    }

#ifdef DEBUG_SETBATCH
    std::cout << "Decided batch: " << this->batch << "\n" << std::endl;
#endif
}

void Gpulet::setRate(model_info model) {
    this->model->execution_rate = getThroughput(model);
}

void Gpulet::printGpuletInfo() {
    // print gpulet0
    std::cout << "GPU ID: " << gpuId << std::endl;
    std::cout << "model_id: " << model->model_id << std::endl;
    std::cout << "model_name: " << model->model_name << std::endl;
    std::cout << "slo: " << model->slo << std::endl;
    std::cout << "rate: " << model->execution_rate << std::endl;

    std::cout << "batch: " << batch << std::endl;
    std::cout << "resource: " << resource << std::endl;
    
    std::cout << std::endl;
}