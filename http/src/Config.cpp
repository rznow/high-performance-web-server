#include "util/Config.h"
#include <fstream>
#include <sstream>

std::string trim(const std::string& str)
{
    size_t begin = 0;
    size_t end = str.size();

    while(begin < end && std::isspace(static_cast<unsigned char>(str[begin])))
        ++begin;

    while(end > begin && std::isspace(static_cast<unsigned char>(str[end - 1])))
        --end;

    return str.substr(begin, end - begin);
}

bool Config::load(const std::string& filename)
{
    std::ifstream file(filename);

    std::string line;
    while(std::getline(file, line))
    {
        //空行和注释行跳过
        if(line.empty() || line[0] == '#')
        {
            continue;
        }

        std::istringstream iss(line);
        std::string key, value;
        if(std::getline(iss, key, '=') && std::getline(iss, value))
        {
            key = trim(key);
            value = trim(value);
            data[key] = value;
        } 
    }

    return true;
}

std::string Config::get(const std::string& key, const std::string& val) const
{
    auto it = data.find(key);

    return it != data.end()? it->second: val;
}

int Config::getInt(const std::string& key, int val) const
{
    auto it = data.find(key);
    if(it == data.end()) return val;

    try{
        return std::stoi(it->second);
    }catch(const std::exception&)
    {
        return val;
    }
}