#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Token.h"

class Lexer {
private:
    std::vector<char> *charList = nullptr;
    int currentIndex = 0;
    bool haveError = false;


private:
    char getNextChar();
    void rollbackLastChar();
    Token analysisNextToken(bool &ignore);

public:
    explicit Lexer(std::vector<char> *charList);
    std::vector<Token> *analysis();
    bool isHaveError() const;
};
