// Config.h
#pragma once

#include <string>
#include <unordered_map>

class Config {
public:
    bool load(const std::string& filename);
    std::string get(const std::string& key, const std::string& val = "") const;
    int getInt(const std::string& key, int val = 0) const;
private:
    std::unordered_map<std::string, std::string> data;
};