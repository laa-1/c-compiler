#pragma once

#include <cstdio>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

class Preprocessor {
private:
    std::unique_ptr<std::ifstream> file = nullptr;
    std::vector<std::vector<char>> *charLineList = new std::vector<std::vector<char>>({{}});

private:
    explicit Preprocessor(std::unique_ptr<std::ifstream> file);
    void matchSingleLineComment(char &lastCharacter, int &countLineFeed);
    void matchMultiLineComment(char &lastCharacter, int &countLineFeed);
    void process();

public:
    static std::vector<std::vector<char>> *process(std::unique_ptr<std::ifstream> file);
};
