#ifndef __GPU_HPP__
#define __GPU_HPP__

#include "gpulet.hpp"

class Gpu {
private:
    // The Id of the GPU
    int gpuId;
    // The available resource of the gpu
    int available_resource;

    Gpulet* first;
    Gpulet* second;
public:
    Gpu(int gpu_num);
    ~Gpu();

    Gpulet* getFirst();
    Gpulet* getSecond();

    void setGpulet(Gpulet gpulet);

    int getAvailableResource();

    bool isFull();
    bool isComplete();

    void printGPUInfo();
};

#endif