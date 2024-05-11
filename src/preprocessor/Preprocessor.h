#pragma once

#include <cstdio>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

class Preprocessor {
private:
    std::unique_ptr<std::ifstream> file = nullptr;

private:
    char matchSingleLineComment();
    char matchMultiLineComment();

public:
    explicit Preprocessor(std::unique_ptr<std::ifstream> file);
    std::vector<char> *process();
};
