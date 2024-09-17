#pragma once

#include <map>
#include "StringConstant.h"

class StringConstantPool {
private:
    std::map<std::string, StringConstant *> map;
    std::uint64_t startAddress = 0;
    std::uint64_t memoryUse = 0;

public:
    StringConstant *operator[](const std::string &value);
    void add(const std::string &value);
    void calculateAddress(std::uint64_t start);
    std::vector<std::uint8_t> serialize();
    [[nodiscard]] std::uint64_t getStartAddress() const;
    [[nodiscard]] std::uint64_t getMemoryUse() const;
};
