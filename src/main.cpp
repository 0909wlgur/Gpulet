#include <fstream>

#include "system.hpp"

int main() {

    readMemoryCsvFile("/root/research/jh/gpulet/data/mem_util.csv");
    readThroughputCsvFile("/root/research/jh/gpulet/data/inference.csv");
    readLatencyCsvFile("/root/research/jh/gpulet/data/inference.csv");
    readInterfereCsvFile("/root/research/jh/gpulet/data/interference.csv");

    // 1. Set the requiring inference data
    std::vector<std::string> model_names = {"resnet50", "vgg19", "densenet121", "mobilenet", "vgg16", "resnet152", "inception", "BERT", "resnet50", "vgg19"};
    std::vector<double> slos = {100, 100, 100, 100, 100, 100, 100, 100, 100, 20};
    std::vector<double> rates = {1000, 1000, 1000, 1000, 1000, 2000, 1000, 100, 1000, 1000};

    // 3. Make System instance
    System* system = new System(model_names, slos, rates);

    system->printPartitioning();

    delete system;
    
    print_interfere_coefficient();

    return 0;
}