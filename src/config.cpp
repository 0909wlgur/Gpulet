#include "config.hpp"

nlohmann::json parseJsonObjects(const std::string& filepath) {
    std::ifstream file(filepath);
    std::vector<nlohmann::json> jsonObjects;
    nlohmann::json config;
    std::string line;
    std::string currentJson;

    assert(file.is_open());

    while (getline(file, line)) {
        try {
            nlohmann::json j = nlohmann::json::parse(line);
            jsonObjects.push_back(j);
        } catch (const nlohmann::json::parse_error& e) {
            // 파싱 에러 처리: 잘못된 JSON 형식 등
            std::cerr << "JSON parsing error on line: " << line << "\n" << e.what() << std::endl;
            // 잘못된 줄을 건너뛰거나, 필요에 따라 추가적인 에러 처리를 수행합니다.
        }
    }

    file.close();

    for (const auto& jsonObj : jsonObjects) {
        for (const auto& item : jsonObj.items()) {
            auto key = item.key();
            auto& value = item.value();
            config[key].merge_patch(value);
        }
    }

    return config;
}