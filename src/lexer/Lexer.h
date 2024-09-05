#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>
#include "Token.h"

class Lexer {
private:
    std::set<std::string> keywordSet = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};
    std::map<std::string, TokenId> keywordTokenIdMap = {{"auto",     TokenId::KEYWORD_AUTO},
                                                        {"break",    TokenId::KEYWORD_BREAK},
                                                        {"case",     TokenId::KEYWORD_CASE},
                                                        {"char",     TokenId::KEYWORD_CHAR},
                                                        {"const",    TokenId::KEYWORD_CONST},
                                                        {"continue", TokenId::KEYWORD_CONTINUE},
                                                        {"default",  TokenId::KEYWORD_DEFAULT},
                                                        {"do",       TokenId::KEYWORD_DO},
                                                        {"double",   TokenId::KEYWORD_DOUBLE},
                                                        {"else",     TokenId::KEYWORD_ELSE},
                                                        {"enum",     TokenId::KEYWORD_ENUM},
                                                        {"extern",   TokenId::KEYWORD_EXTERN},
                                                        {"float",    TokenId::KEYWORD_FLOAT},
                                                        {"for",      TokenId::KEYWORD_FOR},
                                                        {"goto",     TokenId::KEYWORD_GOTO},
                                                        {"if",       TokenId::KEYWORD_IF},
                                                        {"inline",   TokenId::KEYWORD_INLINE},
                                                        {"int",      TokenId::KEYWORD_INT},
                                                        {"long",     TokenId::KEYWORD_LONG},
                                                        {"register", TokenId::KEYWORD_REGISTER},
                                                        {"restrict", TokenId::KEYWORD_RESTRICT},
                                                        {"return",   TokenId::KEYWORD_RETURN},
                                                        {"short",    TokenId::KEYWORD_SHORT},
                                                        {"signed",   TokenId::KEYWORD_SIGNED},
                                                        {"sizeof",   TokenId::KEYWORD_SIZEOF},
                                                        {"static",   TokenId::KEYWORD_STATIC},
                                                        {"struct",   TokenId::KEYWORD_STRUCT},
                                                        {"switch",   TokenId::KEYWORD_SWITCH},
                                                        {"typedef",  TokenId::KEYWORD_TYPEDEF},
                                                        {"union",    TokenId::KEYWORD_UNION},
                                                        {"unsigned", TokenId::KEYWORD_UNSIGNED},
                                                        {"void",     TokenId::KEYWORD_VOID},
                                                        {"volatile", TokenId::KEYWORD_VOLATILE},
                                                        {"while",    TokenId::KEYWORD_WHILE}};
    std::vector<std::vector<char>> *charLineList = nullptr;
    std::vector<Token> *tokenList = new std::vector<Token>();
    char character = '\0';
    std::string buffer;
    int lineIndex = 0;
    int columnIndex = -1;


private:
    explicit Lexer(std::vector<std::vector<char>> *charList);
    inline bool characterIsDigit() const;
    inline bool characterDigitOrLetterOrUnderscore() const;
    inline bool bufferIsKeyword();
    inline void nextCharacter();
    inline void rollbackCharacter();
    Token analysisNextToken(bool &needIgnore);
    void analysis();

public:
    static std::vector<Token> *analysis(std::vector<std::vector<char>> *charLineList);
};
