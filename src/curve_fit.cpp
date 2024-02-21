#include "curve_fit.hpp"

memory_map readMemoryCsvFile(const std::string& filepath) {
    memory_map return_map;
    std::ifstream file(filepath);

    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return return_map;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() == 8) {
            int flag = std::stoi(tokens[7]);
            if (flag != 1) {
                mem_entry entry;
                model_info model;

                std::string model_name = tokens[0];
                int batch = std::stoi(tokens[1]);
                int partition = std::stoi(tokens[2]);
                
                model.model_name = model_name;
                model.batch = batch;
                model.partition = partition;

                double mem_util = std::stod(tokens[4]);
                double l2_util = std::stod(tokens[6]);
                
                entry.Mem_util = mem_util;
                entry.L2_util = l2_util;

                return_map[model] = entry;
            }
        }
    }

    file.close();
    return return_map;
}

latency_map readThroughputCsvFile(const std::string& filepath) {
    latency_map return_map;
    std::ifstream file(filepath);

    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return return_map;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() == 8) {
            int flag = std::stoi((const std::string &)tokens[7]);
            if (flag == 1) {
                model_info model;

                std::string model_name = tokens[0];
                int batch = std::stoi(tokens[1]);
                int partition = std::stoi(tokens[2]);
                
                model.model_name = model_name;
                model.batch = batch;
                model.partition = partition;

                double throughput = std::stod(tokens[4]);

                return_map[model] = throughput;
            }
        }
    }

    file.close();
    return return_map;
}

latency_map readLatencyCsvFile(const std::string& filepath) {
    latency_map return_map;
    std::ifstream file(filepath);

    
    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return return_map;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() == 8) {
            int flag = std::stoi((const std::string &)tokens[7]);
            if (flag == 1) {
                model_info model;

                std::string model_name = tokens[0];
                int batch = std::stoi(tokens[1]);
                int partition = std::stoi(tokens[2]);
                
                model.model_name = model_name;
                model.batch = batch;
                model.partition = partition;

                double latency = std::stod(tokens[6]);

                return_map[model] = latency;
            }
        }
    }

    file.close();
    return return_map;
}

interfere_map readInterfereCsvFile(const std::string& filepath) {
    interfere_map return_map;
    std::ifstream file(filepath);

    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() == 10) {
            int flag = std::stoi((const std::string &)tokens[9]);
            if (flag == 1) {
                model_info model, inter_model;

                std::string model_name = tokens[0];
                int batch = std::stoi(tokens[1]);
                int partition = std::stoi(tokens[2]);

                std::string inter_model_name = tokens[3];
                int inter_batch = std::stoi(tokens[4]);

                double latency = std::stod((const std::string&)tokens[8]);
                // std::cout << latency;
                
                model.model_name = model_name;
                model.batch = batch;
                model.partition = partition;

                inter_model.model_name = inter_model_name;
                inter_model.batch = inter_batch;
                inter_model.partition = 100 - partition;

                return_map[model][inter_model] = latency;
                // std::cout << model.model_name << "|" << inter_model.model_name << "|" << return_map[model][inter_model] << std::endl;
            }
        }
    }

    file.close();
    return return_map;
}

double getThroughput(model_info model) {
    latency_map vgg_throughput_map = readThroughputCsvFile("/root/research/jh/gpulet/implement/data/vgg19_solo.csv");
    latency_map resnet_throughput_map = readThroughputCsvFile("/root/research/jh/gpulet/implement/data/resnet50_solo.csv");

    double return_throughput;

    if (model.model_name == "vgg19") {
        return_throughput = vgg_throughput_map[model];
    } else {
        return_throughput = resnet_throughput_map[model];
    }

    return return_throughput;
}

double getLatency(model_info model){
    latency_map vgg_latency_map = readLatencyCsvFile("/root/research/jh/gpulet/implement/data/vgg19_solo.csv");
    latency_map resnet_latency_map = readLatencyCsvFile("/root/research/jh/gpulet/implement/data/resnet50_solo.csv");

    double return_latency;

    if (model.model_name == "vgg19") {
        return_latency = vgg_latency_map[model];
    } else {
        return_latency = resnet_latency_map[model];
    }

    return return_latency;
}

mem_entry getMemEntry(model_info model) {
    memory_map vgg_mem_map = readMemoryCsvFile("/root/research/jh/gpulet/implement/data/vgg19_with_mem_util.csv");
    memory_map resnet_mem_map = readMemoryCsvFile("/root/research/jh/gpulet/implement/data/resnet50_with_mem_util.csv");

    mem_entry return_mem_entry;

    if (model.model_name == "vgg19") {
        return_mem_entry = vgg_mem_map[model];
    } else {
        return_mem_entry = resnet_mem_map[model];
    }

    return return_mem_entry;
}

void interfere_coefficient_fitting(std::vector<mem_entry> model1, std::vector<mem_entry> model2, std::vector<double> interfere_, double* coefficients) {
    if(model1.size() != model2.size()) {
        std::cerr << "The size of model 1 and model 2 is not matched." << std::endl;
    }
    if(model1.size() != interfere_.size()) {
        std::cerr << "The size of model 1 and interfere_ is not matched." << std::endl;
    }

    int i, n;
    double chisq;
    gsl_matrix *X, *cov;
    gsl_vector *y, *c;

    n = model1.size();

    X = gsl_matrix_alloc(n, 5);
    y = gsl_vector_alloc(n);

    c = gsl_vector_alloc(5);
    cov = gsl_matrix_alloc(5, 5);

    for (i = 0; i < n; i++) {
        double L2m1 = model1[i].L2_util;
        // double L2m1 = i + 1.0;
        double memm1 = model1[i].Mem_util;
        // double memm1 = i + 2.0;

        double L2m2 = model2[i].L2_util;
        // double L2m2 = i - 1.0;
        double memm2 = model2[i].Mem_util;
        // double memm2 = i - 0.5;

        double interfere = interfere_[i];
        // double interfere = (i + 2.45) / 3;

        gsl_matrix_set(X, i, 0, L2m1);
        gsl_matrix_set(X, i, 1, memm1);
        gsl_matrix_set(X, i, 2, L2m2);
        gsl_matrix_set(X, i, 3, memm2);
        gsl_matrix_set(X, i, 4, 1.0);

        gsl_vector_set (y, i, interfere);
    }

    gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc(n, 5);
    gsl_multifit_linear (X, y, c, cov, &chisq, work);
    gsl_multifit_linear_free (work);

    #define C(i) (gsl_vector_get(c,(i)))
    #define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

    // printf ("%g %g %g %g %g\n", C(0), C(1), C(2), C(3), C(4));

    *coefficients = std::abs(C(0));
    *(coefficients + 1) = std::abs(C(1));
    *(coefficients + 2) = std::abs(C(2));
    *(coefficients + 3) = std::abs(C(3));
    *(coefficients + 4) = std::abs(C(4));

    gsl_matrix_free (X);
    gsl_vector_free (y);
    gsl_vector_free (c);
    gsl_matrix_free (cov);
}

void get_interfere_coefficient(double* coefficients) {
    std::vector<std::string> model_names = {"vgg19", "resnet50"};
    std::vector<int> batches = {1, 2, 4, 8, 16, 32 ,64};
    std::vector<int> partitions = {20, 40, 50, 60, 80};

    std::vector<mem_entry> model1, model2;
    std::vector<double> interfere;

    memory_map vgg_mem_map = readMemoryCsvFile("/root/research/jh/gpulet/implement/data/vgg19_with_mem_util.csv");
    memory_map resnet_mem_map = readMemoryCsvFile("/root/research/jh/gpulet/implement/data/resnet50_with_mem_util.csv");
    latency_map vgg_latency_map = readLatencyCsvFile("/root/research/jh/gpulet/implement/data/vgg19_solo.csv");
    latency_map resnet_latency_map = readLatencyCsvFile("/root/research/jh/gpulet/implement/data/resnet50_solo.csv");
    interfere_map vgg_interference_map = readInterfereCsvFile("/root/research/jh/gpulet/implement/data/vgg19_with_interference.csv");
    interfere_map resnet_interference_map = readInterfereCsvFile("/root/research/jh/gpulet/implement/data/resnet50_with_interference.csv");

    for (auto model_name : model_names) {
        for (auto batch : batches) {
            for (auto partition : partitions) {
                // 여기까지가 주요 측정의 context 설정
                model_info model;
                model.model_name = model_name;
                model.batch = batch;
                model.partition = partition;

                int inter_partition = 100 - partition;

                for (auto inter_model_name : model_names) {
                    for (auto inter_model_batch : batches) {
                        model_info inter_model;
                        inter_model.model_name = inter_model_name;
                        inter_model.batch = inter_model_batch;
                        inter_model.partition = inter_partition;

                        mem_entry model1_, model2_;
                        double model_latency, increased_model_latency;

                        if (model_name == "vgg19") {
                            // model -> vgg
                            model1_ = vgg_mem_map[model];
                            model_latency = vgg_latency_map[model];
                            increased_model_latency = vgg_interference_map[model][inter_model];
                            // std::cout << increased_model_latency << std::endl;
                            
                            if (inter_model.model_name == "vgg19") {
                                // inter_model -> vgg
                                model2_ = vgg_mem_map[inter_model];
                            } else {
                                // inter_model -> resnet
                                model2_ = resnet_mem_map[inter_model];
                            }
                        } else {
                            // model -> resnet
                            model1_ = resnet_mem_map[model];
                            model_latency = resnet_latency_map[model];
                            increased_model_latency = resnet_interference_map[model][inter_model];
                            
                            if (inter_model.model_name == "vgg19") {
                                // inter_model -> vgg
                                model2_ = vgg_mem_map[inter_model];
                            } else {
                                // inter_model -> resnet
                                model2_ = resnet_mem_map[inter_model];
                            }
                        }
                        model1.push_back(model1_);
                        model2.push_back(model2_);

                        double interfere_ = increased_model_latency - model_latency;
                        interfere.push_back(interfere_);
                    }
                }
                
            }
        }
    }
    interfere_coefficient_fitting(model1, model2, interfere, coefficients);
}
