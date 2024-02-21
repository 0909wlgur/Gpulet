#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>

std::vector<int> batches = {1, 2, 4, 8, 16, 32, 64};
std::vector<int> partitions = {20, 40, 50, 60, 80, 100};
std::vector<std::string> model_names = {"resnet50"};
std::vector<std::string> infer_model_names = {"vgg19","resnet50","densenet121","inception","mobilenet"};


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

    // 정규 표현식을 사용하여 throughput과 latency 값을 추출
    std::regex pattern_dram(R"(Weighted average of dram__throughput.avg.pct_of_peak_sustained_elapsed: (\d+\.\d+)%)");
    std::regex pattern_lts(R"(Weighted average of lts__t_sectors.avg.pct_of_peak_sustained_elapsed: (\d+\.\d+)%)");

    while (getline(file, line)) {
        std::smatch matches;
        if (std::regex_search(line, matches, pattern_dram) && matches.size() == 2) {
            // dram__throughput 값을 map에 저장
            result["mem_util"] = std::stod(matches[1]);
        } else if (std::regex_search(line, matches, pattern_lts) && matches.size() == 2) {
            // lts__t_sectors 값을 map에 저장
            result["l2_util"] = std::stod(matches[1]);
        }
    }

    file.close();

    if (result.empty()) {
        std::cerr << "The required values could not be found.\n";
    }

    return result;
}

int main() {
    std::string filepath = "/root/research/jh/gpulet/implement/src/";

    std::ofstream output_file("resnet50_with_mem_util.csv"); // 출력 파일 스트림을 생성

    if (!output_file.is_open()) {
        std::cerr << "Output file cannot be opened.\n";
        return -1;
    }

    for (auto model_name : model_names) {
        // file path
        filepath = "/root/research/jh/gpulet/implement/src/";
        std::string basic_filepath = filepath + model_name + "_ncu/" + model_name + "_";

        for (auto batch : batches) {
            // file name
            std::string batch_filepath = basic_filepath + std::to_string(batch) + "_";

            for (auto partition : partitions) {
                // 여기까지가 측정 데이터에 대한 정보

                // file name
                std::string final_filename = batch_filepath + std::to_string(partition) + "_final_result.txt";
                
                std::cout << final_filename << std::endl;

                auto result = processed_data(final_filename);

                if (result.find("error") == result.end()) {
                    if (result["throughput"] == 0 && result["latency"] == 0) {
                        // Case of Out Of Range 
                        output_file << model_name << "," << batch << "," << partition << ",";
                        output_file << "mem_util," << result["mem_util"] << ",l2_util," << result["l2_util"] << ",0" << std::endl;
                    } else {
                        // Standard case
                        output_file << model_name << "," << batch << "," << partition << ",";
                        output_file << "mem_util," << result["mem_util"] << ",l2_util," << result["l2_util"] << ",1" << std::endl;
                    }
                } else {
                    // file patch
                    std::cout << final_filename << std::endl;

                    std::cerr << filepath << " is wrong." << std::endl;
                    return -1;
                }
            }
        }
    }

    output_file.close();

    std::cout << "mem_util.csv has been created.\n";
    return 0;
}