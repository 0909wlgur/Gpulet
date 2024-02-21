#ifndef __CURVE_FIT_HPP__
#define __CURVE_FIT_HPP__

#include <gsl/gsl_multifit.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <map>

#include <assert.h>

#include "definition.hpp"

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

typedef std::map<model_info, mem_entry> memory_map;

// it should be changed to another name
typedef std::map<model_info, double> latency_map;

typedef std::map<model_info, std::map<model_info, double>> interfere_map;

memory_map readMemoryCsvFile(const std::string& filepath);

latency_map readThroughputCsvFile(const std::string& filepath);

latency_map readLatencyCsvFile(const std::string& filepath);

interfere_map readInterfereCsvFile(const std::string& filepath);

double getThroughput(model_info model);

double getLatency(model_info model);

mem_entry getMemEntry(model_info model);

void get_interfere_coefficient(double* coefficients);

void interfere_coefficient_fitting(std::vector<mem_entry> model1, std::vector<mem_entry> model2, std::vector<double> interfere_, double* coefficients);

#endif