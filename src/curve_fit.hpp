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

int readMemoryCsvFile(const std::string& filepath);

int readThroughputCsvFile(const std::string& filepath);

int readLatencyCsvFile(const std::string& filepath);

int readInterfereCsvFile(const std::string& filepath);

double getThroughput(model_info model);

double getLatency(model_info model);

mem_entry getMemEntry(model_info model);

void get_interfere_coefficient(double* coefficients);

void interfere_coefficient_fitting(std::vector<mem_entry> model1, std::vector<mem_entry> model2, std::vector<double> interfere_, double* coefficients);

void print_interfere_coefficient();

#endif