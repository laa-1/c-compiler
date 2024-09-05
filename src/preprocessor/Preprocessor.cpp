#include "Preprocessor.h"

Preprocessor::Preprocessor(std::unique_ptr<std::ifstream> file) : file(std::move(file)) {}

void Preprocessor::matchSingleLineComment(char &lastCharacter, int &countLineFeed) {
    char ch;
    countLineFeed = 0;
    while (true) {
        ch = static_cast<char>(file->get());
        if (ch == '\n') {
            countLineFeed++;
            lastCharacter = ch;
            return;
        }
        if (ch == EOF) {
            lastCharacter = ch;
            return;
        }
    }
}

void Preprocessor::matchMultiLineComment(char &lastCharacter, int &countLineFeed) {
    char ch1 = '\0';
    char ch2;
    countLineFeed = 0;
    while (true) {
        ch2 = static_cast<char>(file->get());
        if ((ch1 == '*' && ch2 == '/') || ch2 == EOF) {
            lastCharacter = ch2;
            return;
        }
        if (ch2 == '\n') {
            countLineFeed++;
        }
        ch1 = ch2;
    }
}

void Preprocessor::process() {
    char ch1 = '\0';
    char ch2;
    int countLineFeed;
    while (true) {
        ch2 = static_cast<char>(file->get());
        if (ch1 == '/' && ch2 == '/') {
            charLineList->back().pop_back();
            matchSingleLineComment(ch2, countLineFeed);
            if (ch2 == EOF) {
                return;
            }
            for (int i = 0; i < countLineFeed; i++) {
                charLineList->emplace_back();
            }
            ch1 = ch2;
        } else if (ch1 == '/' && ch2 == '*') {
            charLineList->back().pop_back();
            matchMultiLineComment(ch2, countLineFeed);
            if (ch2 == EOF) {
                return;
            }
            for (int i = 0; i < countLineFeed; i++) {
                charLineList->emplace_back();
            }
            ch1 = ch2;
        } else if (ch2 == EOF) {
            charLineList->back().push_back(ch2);
            return;
        } else if (ch2 == '\n') {
            charLineList->emplace_back();
            ch1 = ch2;
        } else {
            charLineList->back().push_back(ch2);
            ch1 = ch2;
        }
    }
}

std::vector<std::vector<char>> *Preprocessor::process(std::unique_ptr<std::ifstream> file) {
    auto *preprocessor = new Preprocessor(std::move(file));
    preprocessor->process();
    std::vector<std::vector<char>> *charLineList = preprocessor->charLineList;
    delete preprocessor;
    return charLineList;
}

