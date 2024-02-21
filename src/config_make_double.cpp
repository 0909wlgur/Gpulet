#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include "nlohmann/json.hpp"

std::map<std::string, double> processed_data(const std::string& filepath) {
    std::ifstream file(filepath);
    std::ofstream output_file;
    std::string line;
    std::string lastLine;
    std::map<std::string, double> result;

    if (!file.is_open()) {
        std::cerr << "File cannot be opened.\n";
        result["error"] = -1;
        return result;
    }

    // 파일의 마지막 줄을 읽기
    while (getline(file, line)) {
        if (!line.empty()) {
            lastLine = line;
        }
    }

    file.close();

    // 정규 표현식을 사용하여 throughput과 latency 값을 추출
    std::regex pattern(R"(\bthroughput: (\d+\.\d+), Total inference latency: (\d+\.\d+))");
    std::smatch matches;

    if (std::regex_search(lastLine, matches, pattern) && matches.size() == 3) {
        // JSON 객체 생성
        result["throughput"] = std::stod(matches[1]);
        result["latency"] = std::stod(matches[2]);
    } else {
        std::cerr << "The required values could not be found.\n";
    }

    return result;
}

int main() {
    nlohmann::json finalJson;
    std::string filepath;
    
    std::vector<std::string> model_names = {"vgg19", "resnet50", "densenet121", "mobilenet", "inception"};

    // std::map<std::string, std::vector<std::string>> layer_map;
    // std::vector<std::string> resnet_layers = {"50", "101", "152"};
    // std::vector<std::string> densenet_layers = {"121", "169", "201"};
    // std::vector<std::string> vgg_layers = {"19"};
    // std::vector<std::string> mobilenet_layers = {"1"};
    // std::vector<std::string> inception_layers = {"1"};

    // layer_map["vgg"] = vgg_layers;
    // layer_map["resnet"] = resnet_layers;
    // layer_map["densenet"] = densenet_layers;
    // layer_map["mobilenet"] = mobilenet_layers;
    // layer_map["inception"] = inception_layers;

    std::vector<int> partitions = {20, 40, 50, 60, 80, 100};
    std::vector<int> batches = {1, 2, 4, 8, 16, 32, 64};

    for (auto model : model_names) {
        for (auto batch : batches) {
            for (auto partition : partitions) {
                std::string filename = model + "_" + std::to_string(batch) + "_" + std::to_string(partition);
                
                filepath = "/data/extHome/chhong/research/jihyuk/sc/profiling/data/double/" + model + "/" \
                            + std::to_string(batch) + "/" + std::to_string(partition) + "/";

                int inter_partition = 100 - partition;

                for (auto inter_model : model_names) {
                    for (auto inter_batch : batches) {
                        filename += "_infer_by_" + inter_model + "_" + std::to_string(batch);
                        filepath += filename;
                        std::cout << filename << std::endl;

                        auto result = processed_data(filepath);

                        if (result.find("error") == result.end()) {
                            std::cerr << "The value of the " << filepath << " is wrong." << std::endl;
                            return -1;
                        } else {
                            finalJson[model][std::to_string(batch)][std::to_string(partition)] = result;
                        }
                    }
                }                
            }
        }
    }

    // JSON 파일로 최종 결과 저장
    std::ofstream jsonFile("mps_double_config.json");
    jsonFile << finalJson.dump(4);
    jsonFile.close();

    std::cout << "mps_double_config.json has been created.\n";
    return 0;
}