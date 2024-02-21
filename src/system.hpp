#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__


#include <vector>
#include <memory>
#include <utility>
#include <algorithm>

#include "config.hpp"
#include "gpu.hpp"

class System {
private:
    std::vector<model_inference_info*> model_list;
    std::vector<Gpu*> gpu_list;
    
    // total GPU count
    int maxGPU;
    // Number of available gpulets now 
    int cnt_gpulet;
    
    int finalGPU;
public:
    // Make model list with the input vectors
    // input = (vector of model name, vector of model rate, vector of model slo)
    System(const std::vector<std::string>& model_names, 
                const std::vector<int>& model_rates, const std::vector<int>& model_slos, int gpu_num);

    // Execute the elastic partitioning(algorithm 1 of the paper)
    // input = (config instance)
    void elastricPartitioning();
    // Find the best fit of the gpulet
    Gpulet findBestFit(int model_id, int p_ideal);

    // Create a list targeting gpulets that can be inserted.
    std::vector<std::shared_ptr<Gpulet>> make_available_gpulet_list();

    // Get model_list[order].execution_rate
    double get_remain_rate(int order);
    // Returns true if the gpulet can be added, or returns false if it cannot be added.
    bool is_remain_gpulet();

    // Get required partition rate
    int get_req_partition_rate(const std::string& model_name, double rate);
    // Get efficient partition rate
    int get_eff_partition_rate(const std::string& model_name);

    void checkGPU();

    void printModelList();
    void printPartitioning();
};

#endif