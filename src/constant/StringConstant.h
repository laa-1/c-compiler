#pragma once

#include <string>
#include <cstdint>
#include <utility>
#include <vector>

struct StringConstant {
public:
    std::string value;
    std::uint64_t address = 0;

    StringConstant() = default;

    explicit StringConstant(std::string value) : value(std::move(value)) {}
};
