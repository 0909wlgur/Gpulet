#ifndef __GPULET_HPP__
#define __GPULET_HPP__

#include <string>
#include <nlohmann/json.hpp>

#include "curve_fit.hpp"

typedef struct model_inference_info_ {
    // model_name = model's name
    std::string model_name;
    
    // model_id = received order which user required model inferences.
    int model_id;

    // rate = required total rate from user 
    double execution_rate;

    // slo[ms] = required slo from user
    double slo;
} model_inference_info;

class Gpulet {
private:
    model_inference_info* model;

    // batch = determined batch size from the igniter equation
    int batch;

    // resource = determined resource from the igniter equation.
    int resource;

    // gpuId = the place of this gpulet exist.
    int gpuId;
public:
    // make gpulet with available_resource
    Gpulet(int available_resource, int gpuId_ = -1);
    ~Gpulet();

    // Return the gpu id
    int getGPUId();
    // Return the partition size of the gpulet
    int getPartition();
    // Return the batch size of the gpulet
    int getBatch();
    // Return the throughput of the gpulet
    double getRate();
    // Return the model inference info
    model_inference_info getModelInfo();

    // split and allocate the gpulet to the p_ideal size partition
    Gpulet* splitAndAllocate(model_inference_info model, int p_ideal);

    // fix the partition size about the gpulet
    void fixPartition(int p_ideal);
    // fix the batch size about the gpulet
    void fixBatch(int batch);
    // fix the Rate about the gpulet
    void fixRate(double rate);
    // fix the information about the gpulet
    void fixGpulet(model_inference_info model);
    

    // Set the batch size of the gpulet
    void setBatch(std::vector<model_info> model_info_vector);
    // Set the rate of the gpulet
    void setRate(model_info model);

    void printGpuletInfo();
};



#endif