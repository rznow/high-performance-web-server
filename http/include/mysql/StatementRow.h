#pragma once
#include <vector>
class StatementRow
{
public:
    int getInt(int index) const
    {
        return *reinterpret_cast<int*>(
            (*buffers)[index].data());
    }

    std::string getString(int index) const
    {
        if((*nulls)[index])
            return "";

        return std::string(
            (*buffers)[index].data(),
            (*lengths)[index]);
    }

    bool isNull(int index) const
    {
        return (*nulls)[index];
    }

private:

    friend class Statement;

    std::vector<int>* ints = nullptr;

    std::vector<std::vector<char>>* buffers = nullptr;

    std::vector<unsigned long>* lengths = nullptr;

    std::vector<char>* nulls = nullptr;
};