#ifndef __DEFINITION_HPP__
#define __DEFINITION_HPP__

// #define DEBUG_PARTITION

typedef struct model_info_ {
    std::string model_name;
    int batch;
    int partition;

    bool operator<(const model_info_& other) const {
        if (model_name != other.model_name)
            return model_name < other.model_name;
        if (batch != other.batch)
            return batch < other.batch;
        return partition < other.partition;
    }
} model_info;

typedef struct mem_entry_ {
    double Mem_util;
    double L2_util;
} mem_entry;

extern std::map<model_info, mem_entry> memory_map;

extern std::map<model_info, double> throughput_map;
extern std::map<model_info, double> latency_map;

extern std::map<model_info, std::map<model_info, double>> interfere_map;

#endif