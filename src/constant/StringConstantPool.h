#pragma once

#include <map>
#include "StringConstant.h"

/**
 * 字符串常量池类。
 */
class StringConstantPool {
private:
    std::map<std::string, StringConstant *> map;
    std::uint64_t startAddress = 0;
    std::uint64_t memoryUse = 0;

public:
    StringConstant *operator[](const std::string &value);
    void add(const std::string &value);
    void calculateAddress(std::uint64_t start);
    // 序列化为字节数组形式，作为运行时的数据区的一部分
    std::vector<std::uint8_t> serialize();
    std::uint64_t getStartAddress() const;
    std::uint64_t getMemoryUse() const;
};
