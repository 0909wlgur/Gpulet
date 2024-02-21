#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include "nlohmann/json.hpp"

std::map<std::string, double> processed_data(const std::string& filepath) {
    std::ifstream file(filepath);
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
    std::regex pattern(R"(\bWeighted average of dram__throughput.avg.pct_of_peak_sustained_elapsed: ([0-9.]+)%\nWeighted average of lts__t_sectors.avg.pct_of_peak_sustained_elapsed: ([0-9.]+)%)");
    std::smatch matches;

    if (std::regex_search(lastLine, matches, pattern) && matches.size() == 3) {
        // JSON 객체 생성
        result["mem_util"] = std::stod(matches[1]);
        result["l2_util"] = std::stod(matches[2]);
    } else {
        std::cerr << "The required values could not be found.\n";
    }

    return result;
}

int main() {
    std::string basic_file_path = "/data/extHome/chhong/research/jihyuk/sc/profiling/data/solo";
    std::vector<std::string> model_names = {"vgg", "resnet", "densenet", "mobilenet", "inception"};
    nlohmann::json finalJson;

    std::map<std::string, std::vector<std::string>> layer_map;
    std::vector<std::string> resnet_layers = {"50", "101", "152"};
    std::vector<std::string> densenet_layers = {"121", "169", "201"};
    std::vector<std::string> vgg_layers = {"19"};
    std::vector<std::string> mobilenet_layers = {"1"};
    std::vector<std::string> inception_layers = {"1"};

    layer_map["vgg"] = vgg_layers;
    layer_map["resnet"] = resnet_layers;
    layer_map["densenet"] = densenet_layers;
    layer_map["mobilenet"] = mobilenet_layers;
    layer_map["inception"] = inception_layers;

    std::vector<int> partitions = {20, 40, 50, 60, 80, 100};
    std::vector<int> batchs = {1, 2, 4, 8, 16, 32, 64, 128, 256};

    for(auto model : model_names) {
        auto model_layers = layer_map[model];
        for(auto layer : model_layers) {
            for(auto batch : batchs) {
                for(auto partition : partitions) {
                    std::string filename = model + "_" + layer + "_" + std::to_string(batch) + "_" + std::to_string(partition) + ".txt";
                    std::string filepath;

                    if (model == "inception" | model == "mobilenet") {
                        filepath = "/data/extHome/chhong/research/jihyuk/sc/profiling/data/double/" + model + "/" + filename;
                    } else {
                        filepath = "/data/extHome/chhong/research/jihyuk/sc/profiling/data/double/" + model + layer + "/" + filename;
                    }
                    
                    auto result = processed_data(filepath);

                    if (result.find("error") == result.end()) {
                        std::cerr << "The value of the " << filepath << " is wrong." << std::endl;
                        return -1;
                    } else {
                        finalJson[model][layer][std::to_string(batch)][std::to_string(partition)] = result;
                    }
                    std::cout << filepath << std::endl;
                }
            }
        }
    }

    // JSON 파일로 최종 결과 저장
    std::ofstream jsonFile("mps_solo_config.json");
    jsonFile << finalJson.dump(4);
    jsonFile.close();

    std::cout << "Final JSON file has been created.\n";
    return 0;
}