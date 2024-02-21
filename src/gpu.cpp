#include "gpu.hpp"

Gpu::Gpu(int gpu_num) 
    : gpuId(gpu_num), available_resource(100) {
    first = new Gpulet(available_resource, gpuId);
    second = nullptr;
}

Gpu::~Gpu() {
    delete first;
    delete second;
}

Gpulet* Gpu::getFirst() {
    return first;
}

Gpulet* Gpu::getSecond() {
    return second;
}

void Gpu::setGpulet(Gpulet gpulet) {
    if (second == nullptr) {
        // modify first
        
        first->fixGpulet(gpulet.getModelInfo());
        first->fixPartition(gpulet.getPartition());
        first->fixBatch(gpulet.getBatch());
        first->fixRate(gpulet.getRate());
        
        available_resource -= gpulet.getPartition();
        
        // std::cout << "gpuId: " << gpuId << std::endl;
        second = new Gpulet(available_resource, gpulet.getGPUId());
        // second->printGpuletInfo();
    } else {
        // modify second
        // std::cout << "modify second\n" << std::endl;

        second->fixGpulet(gpulet.getModelInfo());
        second->fixBatch(gpulet.getBatch());
        second->fixRate(gpulet.getRate());
        available_resource = 0;

#ifdef DEBUG_PARTITION
        std::cout << "GPU" << gpuId <<" is full now." << std::endl << std::endl;
#endif
    }
}

int Gpu::getAvailableResource() {
    return available_resource;
}

bool Gpu::isFull() {
    if (second != nullptr)
        return true;
    else 
        return false;
}

bool Gpu::isComplete() {
    if (second->getGPUId() != -1)
        return true;
    else 
        return false;
}

void Gpu::printGPUInfo() {
    std::cout << "GPU: " << gpuId << std::endl;
    std::cout << "First" << std::endl;
    first->printGpuletInfo();

    if (second->getBatch() != 0) {
        std::cout << "Second" << std::endl;
        second->printGpuletInfo();
    }
    std::cout << "---------------------" << std::endl;
}