#include "StringConstantPool.h"
#include <algorithm>

StringConstant *StringConstantPool::operator[](const std::string &value) {
    if (map.contains(value)) {
        return map[value];
    }
    return nullptr;
}

void StringConstantPool::add(const std::string &value) {
    if (!map.contains(value)) {
        map[value] = new StringConstant(value);
    }
}

void StringConstantPool::calculateAddress(std::uint64_t start) {
    startAddress = start;
    std::uint64_t currentAddress = start;
    for (const auto &pair : map) {
        StringConstant *stringConstant = pair.second;
        stringConstant->address = currentAddress;
        currentAddress += stringConstant->value.size() + 1; // 预留末尾的'\0'
    }
    memoryUse = currentAddress - start;
}

std::vector<std::uint8_t> StringConstantPool::serialize() {
    std::vector<std::uint8_t> byteList(memoryUse, 0);
    for (const auto &pair : map) {
        StringConstant *stringConstant = pair.second;
        for (int i = 0; i < stringConstant->value.size(); i++) {
            byteList[stringConstant->address - startAddress + i] = (std::uint8_t) (stringConstant->value[i]);
        }
    }
    return byteList;
}

std::uint64_t StringConstantPool::getStartAddress() const {
    return startAddress;
}

std::uint64_t StringConstantPool::getMemoryUse() const {
    return memoryUse;
}
