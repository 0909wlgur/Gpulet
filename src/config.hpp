#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <nlohmann/json.hpp>
#include <assert.h>

// batchsize:partition_size:rate

nlohmann::json parseJsonObjects(const std::string& filepath);


#endif