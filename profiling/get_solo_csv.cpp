#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>

std::vector<int> batches = {1, 2, 4, 8, 16, 32, 64};
std::vector<int> partitions = {20, 40, 50, 60, 80, 100};
std::vector<std::string> model_names = {"vgg16", "vgg19", "resnet50", "resnet101", "resnet152", "densenet121", "densenet169", "densenet201", "inception", "mobilenet"};

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
        result["throughput"] = 0;
        result["latency"] = 0;
    }

    return result;
}

int main() {
    std::string filepath = "/root/research/jh/gpulet/profiling/solo/";

    std::ofstream output_file("solo.csv"); // 출력 파일 스트림을 생성

    if (!output_file.is_open()) {
        std::cerr << "Output file cannot be opened.\n";
        return -1;
    }

    for (auto model_name : model_names) {
        // file path
        std::string model_filepath = filepath + model_name + "/" + model_name + "_";

        for (auto batch : batches) {
            // file name
            std::string batch_filepath = model_filepath + std::to_string(batch) + "_";

            for (auto partition : partitions) {
                // file name
                std::string final_filename = batch_filepath + std::to_string(partition) + ".txt";
                
                // std::cout << final_filename << std::endl;

                auto result = processed_data(final_filename);

                if (result.find("error") == result.end()) {
                    if (result["throughput"] == 0 && result["latency"] == 0) {
                        // Case of Out Of Range 
                        output_file << model_name << "," << batch << "," << partition << ",";
                        output_file << "throughput," << result["throughput"] << ",latency," << result["latency"] << ",0" << std::endl;
                    } else {
                        // Standard case
                        output_file << model_name << "," << batch << "," << partition << ",";
                        output_file << "throughput," << result["throughput"] << ",latency," << result["latency"] << ",1" << std::endl;
                    }
                } else {
                    // file patch
                    std::cerr << final_filename << " is wrong." << std::endl;
                    return -1;
                }
            }
        }
    }

    output_file.close();

    std::cout << "vgg19_solo.csv has been created.\n";
    return 0;
}