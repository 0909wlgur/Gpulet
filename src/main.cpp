#include <fstream>

#include "system.hpp"

int main() {
    // 1. Set the requiring inference data
    std::vector<std::string> model_names = {"resnet50", "vgg19", "resnet50", "vgg19"};
    std::vector<int> slos = {100, 100, 100, 100};
    std::vector<int> rates = {1000, 1000, 1000, 1000};

    // 3. Make System instance
    int gpu_num = 10;
    System* system = new System(model_names, slos, rates, gpu_num);

    system->printPartitioning();

    delete system;
    
    return 0;
}