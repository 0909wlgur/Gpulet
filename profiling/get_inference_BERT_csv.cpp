#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>

std::vector<int> batches = {1, 2, 4, 8, 16, 32, 64};
std::vector<int> partitions = {20, 40, 50, 60, 80, 100};
std::vector<std::string> model_names = {"BERT"};

std::map<std::string, double> processed_data(const std::string& filepath) {
    std::ifstream file(filepath);
    std::map<std::string, double> result;

    if (!file.is_open()) {
        std::cerr << "File cannot be opened: " << filepath << "\n";
        return result;
    }

    std::string line;
    // 정규 표현식 패턴 정의
    std::regex latency_pattern(R"(\binfer:latency\(ms\):avg = (\d+\.\d+))");
    std::regex throughput_pattern(R"(\binfer:throughput\(samples/s\):avg = (\d+\.\d+))");

    while (getline(file, line)) {
        std::smatch matches;
        // Latency 검출
        if (std::regex_search(line, matches, latency_pattern) && matches.size() == 2) {
            result["latency"] = std::stod(matches[1].str());
        }
        // Throughput 검출
        else if (std::regex_search(line, matches, throughput_pattern) && matches.size() == 2) {
            result["throughput"] = std::stod(matches[1].str());
        }
    }

    file.close();

    // 검출된 결과가 없으면 에러 메시지 출력
    if (result.empty()) {
        std::cerr << "The required values could not be found in: " << filepath << "\n";
    }

    return result;
}

int main() {
    std::string filepath = "/root/research/jh/gpulet/profiling/solo/";

    std::ofstream output_file("inference_BERT.csv"); // 출력 파일 스트림을 생성

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

    std::cout << "inference_BERT.csv has been created.\n";
    return 0;
}