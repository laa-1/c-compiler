#include "Preprocessor.h"

Preprocessor::Preprocessor(std::unique_ptr<std::ifstream> file) : file(std::move(file)) {}

char Preprocessor::matchSingleLineComment() {
    while (true) {
        char currentChar = static_cast<char>(file->get());
        if (currentChar == '\n' || currentChar == EOF) {
            return currentChar;
        }
    }
}

char Preprocessor::matchMultiLineComment() {
    char currentChar;
    char lastChar = '\0';
    while (true) {
        currentChar = static_cast<char>(file->get());
        if (lastChar == '*' && currentChar == '/') {
            return currentChar;
        }
        if (currentChar == EOF) {
            return currentChar;
        }
        lastChar = currentChar;
    }
}

std::vector<char> *Preprocessor::process() {
    auto *charList = new std::vector<char>();
    char currentChar;
    char lastChar = '\0';
    while (true) {
        currentChar = static_cast<char>(file->get());
        if (lastChar == '/' && currentChar == '/') {
            lastChar = matchSingleLineComment();
            charList->pop_back();
        } else if (lastChar == '/' && currentChar == '*') {
            lastChar = matchMultiLineComment();
            charList->pop_back();
        } else if (currentChar == EOF) {
            charList->push_back(EOF);
            return charList;
        } else {
            charList->push_back(currentChar);
            lastChar = currentChar;
        }
    }
}

