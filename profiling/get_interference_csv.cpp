#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>

std::vector<int> batches = {1, 2, 4, 8, 16, 32, 64};
std::vector<int> partitions = {20, 40, 50, 60, 80};
std::vector<std::string> model_names = {"vgg19", "resnet50", "densenet121"};
std::vector<std::string> infer_model_names = {"vgg19","resnet50", "densenet121", "mobilenet", "inception"};


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
        std::cout << filepath << std::endl;
        result["throughput"] = 0;
        result["latency"] = 0;
    }

    return result;
}

int main() {
    std::string filepath = "/root/research/jh/gpulet/profiling/double/";

    std::ofstream output_file("interference.csv"); // 출력 파일 스트림을 생성

    if (!output_file.is_open()) {
        std::cerr << "Output file cannot be opened.\n";
        return -1;
    }

    for (auto model_name : model_names) {
        // file path
        std::string basic_filepath = filepath + model_name + "/";

        // file name
        std::string basic_filename = model_name + "_";

        for (auto batch : batches) {
            // file path
            std::string batch_filepath = basic_filepath + std::to_string(batch) + "/";

            // file name
            std::string batch_filename = basic_filename + std::to_string(batch) + "_";

            for (auto partition : partitions) {
                // 여기까지가 측정 데이터에 대한 정보
                // file path
                std::string final_filepath = batch_filepath + std::to_string(partition) + "/";

                // file name
                std::string partition_filename = batch_filename + std::to_string(partition) + "_infer_by_";

                for (auto infer_model_name : infer_model_names) {
                    // file name
                    std::string infer_model_filename = partition_filename + infer_model_name +"_";

                    for (auto infer_model_batch : batches) {
                        // file name
                        std::string final_filename = infer_model_filename + std::to_string(infer_model_batch) + ".txt";
                        std::string filepath = final_filepath + final_filename;

                        auto result = processed_data(filepath);

                        if (result.find("error") == result.end()) {
                            if (result["throughput"] == 0 && result["latency"] == 0) {
                                // Case of Out Of Range 
                                output_file << model_name << "," << batch << "," << partition << ",";
                                output_file << infer_model_name << "," << infer_model_batch << ",";
                                output_file << "throughput," << result["throughput"] << ",latency," << result["latency"] << ",0" << std::endl;
                            } else {
                                // Standard case
                                output_file << model_name << "," << batch << "," << partition << ",";
                                output_file << infer_model_name << "," << infer_model_batch << ",";
                                output_file << "throughput," << result["throughput"] << ",latency," << result["latency"] << ",1" << std::endl;
                            }
                        } else {
                            // file patch
                            std::cout << final_filepath << std::endl;
                            std::cout << final_filename << std::endl;

                            std::cerr << filepath << " is wrong." << std::endl;
                            return -1;
                        }
                    }
                }
            }
        }
    }
    

    output_file.close();

    std::cout << "interference.csv has been created.\n";
    return 0;
}