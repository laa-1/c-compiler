#include "Lexer.h"

#include <string>
#include <map>
#include <set>
#include "../logger/Logger.h"

bool isDigit(const char &ch) {
    return ch >= 48 && ch <= 57;
}

bool isDigitLetterUnderscore(const char &ch) {
    return (ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || ch == 95;
}

bool isKeyword(const std::string &str) {
    static std::set<std::string> keywordSet = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};
    return keywordSet.find(str) != keywordSet.end();
}

TokenId keywordToTokenId(const std::string &keyword) {
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
    return keywordTokenIdMap[keyword];
}

Lexer::Lexer(std::vector<char> *charList) : charList(charList) {}

char Lexer::getNextChar() {
    return (*charList)[currentIndex++];
}

void Lexer::rollbackLastChar() {
    currentIndex--;
}

Token Lexer::analysisNextToken(bool &ignore) {
    char ch;
    std::string buffer;
    ch = getNextChar();
    switch (ch) {
        case EOF:
            return {TokenId::SPECIAL_EOF, ""};
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            ignore = true;
            return {};
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
            // 匹配标识符和关键字
            do {
                buffer += ch;
                ch = getNextChar();
            } while (isDigitLetterUnderscore(ch));
            rollbackLastChar();
            if (isKeyword(buffer)) {
                // 关键字
                return {keywordToTokenId(buffer), buffer};
            } else {
                // 标识符
                return {TokenId::IDENTIFIER, buffer};
            }
        case '0':
            // 匹配0开头的数值常量
            buffer += ch;
            ch = getNextChar();
            if (ch == '.') {
                // 匹配浮点数常量
                buffer += ch;
                ch = getNextChar();
                if (!isDigit(ch)) {
                    rollbackLastChar();
                    haveError = true;
                    Logger::error("numerical constant cannot end with a dot");
                    return {};
                }
                while (isDigit(ch)) {
                    buffer += ch;
                    ch = getNextChar();
                }
                rollbackLastChar();
                return {TokenId::LITERAL_FLOATING_POINT, buffer};
            } else {
                // 数值0
                rollbackLastChar();
                return {TokenId::LITERAL_INTEGER, buffer};
            }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            // 匹配整形常量
            while (isDigit(ch)) {
                buffer += ch;
                ch = getNextChar();
            }
            if (ch == '.') {
                // 匹配浮点数常量
                buffer += ch;
                ch = getNextChar();
                if (!isDigit(ch)) {
                    rollbackLastChar();
                    haveError = true;
                    Logger::error("numerical constant cannot end with a dot");
                    return {};
                } else {
                    while (isDigit(ch)) {
                        buffer += ch;
                        ch = getNextChar();
                    }
                    rollbackLastChar();
                    return {TokenId::LITERAL_FLOATING_POINT, buffer};
                }
            } else {
                // 整形常量
                rollbackLastChar();
                return {TokenId::LITERAL_INTEGER, buffer};
            }
        case '\'':
            // 匹配字符常量
            ch = getNextChar();
            if (ch == '\\') {
                // 匹配转义字符
                ch = getNextChar();
                switch (ch) {
                    case 'a':
                        buffer += '\a';
                        break;
                    case 'b':
                        buffer += '\b';
                        break;
                    case 'f':
                        buffer += '\f';
                        break;
                    case 'n':
                        buffer += '\n';
                        break;
                    case 'r':
                        buffer += '\r';
                        break;
                    case 't':
                        buffer += '\t';
                        break;
                    case 'v':
                        buffer += '\v';
                        break;
                    case '0':
                        buffer += '\0';
                        break;
                    default:
                        buffer += ch;
                        break;
                }
            } else if (ch == '\'') {
                haveError = true;
                Logger::error("character constants are not allowed to have more than one character");
                return {};
            } else {
                buffer += ch;
            }
            ch = getNextChar();
            if (ch != '\'') {
                rollbackLastChar();
                haveError = true;
                Logger::error("character constants are not allowed to have more than one character");
                return {};
            }
            return {TokenId::LITERAL_CHARACTER, buffer};
        case '"':
            // 匹配字符串字面量
            while (true) {
                ch = getNextChar();
                if (ch == '\\') {
                    // 匹配转义字符
                    ch = getNextChar();
                    switch (ch) {
                        case 'a':
                            buffer += '\a';
                            break;
                        case 'b':
                            buffer += '\b';
                            break;
                        case 'f':
                            buffer += '\f';
                            break;
                        case 'n':
                            buffer += '\n';
                            break;
                        case 'r':
                            buffer += '\r';
                            break;
                        case 't':
                            buffer += '\t';
                            break;
                        case 'v':
                            buffer += '\v';
                            break;
                        case '0':
                            buffer += '\0';
                            break;
                        default:
                            buffer += ch;
                            break;
                    }
                } else if (ch == '"') {
                    return {TokenId::LITERAL_STRING, buffer};
                } else if (ch == EOF) {
                    haveError = true;
                    Logger::error("string constants cannot be used without the right double quote");
                    return {};
                } else {
                    buffer += ch;
                }
            }
        case '+':
            ch = getNextChar();
            if (ch == '+') {
                return {TokenId::PUNCTUATOR_INCREMENT, "++"};
            } else if (ch == '=') {
                return {TokenId::PUNCTUATOR_ADD_ASSIGN, "+="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_ADD, "+"};
            }
        case '-':
            ch = getNextChar();
            if (ch == '-') {
                return {TokenId::PUNCTUATOR_DECREMENT, "--"};
            } else if (ch == '=') {
                return {TokenId::PUNCTUATOR_SUB_ASSIGN, "-="};
            } else if (ch == '>') {
                return {TokenId::PUNCTUATOR_POINT_TO, "->"};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_SUB, "-"};
            }
        case '*':
            ch = getNextChar();
            if (ch == '=') {
                return {TokenId::PUNCTUATOR_MUL_ASSIGN, "*="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_MUL, "*"};
            }
        case '/':
            ch = getNextChar();
            if (ch == '=') {
                return {TokenId::PUNCTUATOR_DIV_ASSIGN, "/="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_DIV, "/"};
            }
        case '%':
            ch = getNextChar();
            if (ch == '=') {
                return {TokenId::PUNCTUATOR_MOD_ASSIGN, "%="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_MOD, "%"};
            }
        case '=':
            ch = getNextChar();
            if (ch == '=') {
                return {TokenId::PUNCTUATOR_EQUAL, "=="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_ASSIGN, "="};
            }
        case '>':
            ch = getNextChar();
            if (ch == '>') {
                ch = getNextChar();
                if (ch == '=') {
                    return {TokenId::PUNCTUATOR_RIGHT_SHIFT_ASSIGN, ">>="};
                } else {
                    rollbackLastChar();
                    return {TokenId::PUNCTUATOR_RIGHT_SHIFT, ">>"};
                }
            } else if (ch == '=') {
                return {TokenId::PUNCTUATOR_GREATER_EQUAL, ">="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_GREATER, ">"};
            }
        case '<':
            ch = getNextChar();
            if (ch == '<') {
                ch = getNextChar();
                if (ch == '=') {
                    return {TokenId::PUNCTUATOR_LEFT_SHIFT_ASSIGN, "<<="};
                } else {
                    rollbackLastChar();
                    return {TokenId::PUNCTUATOR_LEFT_SHIFT, "<<"};
                }
            } else if (ch == '=') {
                return {TokenId::PUNCTUATOR_LESS_EQUAL, "<="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_LESS, "<"};
            }
        case '!':
            ch = getNextChar();
            if (ch == '=') {
                return {TokenId::PUNCTUATOR_LOGICAL_NOT_EQUAL, "!="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_LOGICAL_NOT, "!"};
            }
        case '&':
            ch = getNextChar();
            if (ch == '&') {
                return {TokenId::PUNCTUATOR_LOGICAL_AND, "&&"};
            } else if (ch == '=') {
                return {TokenId::PUNCTUATOR_BITWISE_AND_ASSIGN, "&="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_BITWISE_AND, "&"};
            }
        case '|':
            ch = getNextChar();
            if (ch == '|') {
                return {TokenId::PUNCTUATOR_LOGICAL_OR, "||"};
            } else if (ch == '=') {
                return {TokenId::PUNCTUATOR_BITWISE_OR_ASSIGN, "|="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_BITWISE_OR, "|"};
            }
        case '^':
            ch = getNextChar();
            if (ch == '=') {
                return {TokenId::PUNCTUATOR_EXCLUSIVE_OR_ASSIGN, "^="};
            } else {
                rollbackLastChar();
                return {TokenId::PUNCTUATOR_EXCLUSIVE_OR, "^"};
            }
        case '~':
            return {TokenId::PUNCTUATOR_BITWISE_NOT, "~"};
        case '?':
            return {TokenId::PUNCTUATOR_QUESTION, "?"};
        case ':':
            return {TokenId::PUNCTUATOR_COLON, ":"};
        case '(':
            return {TokenId::PUNCTUATOR_LEFT_PARENTHESES, "("};
        case ')':
            return {TokenId::PUNCTUATOR_RIGHT_PARENTHESES, ")"};
        case '[':
            return {TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS, "["};
        case ']':
            return {TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS, "]"};
        case '{':
            return {TokenId::PUNCTUATOR_LEFT_CURLY_BRACES, "{"};
        case '}':
            return {TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES, "}"};
        case ',':
            return {TokenId::PUNCTUATOR_COMMA, ","};
        case ';':
            return {TokenId::PUNCTUATOR_SEMICOLON, ";"};
        case '.':
            return {TokenId::PUNCTUATOR_DOT, "."};
        case '#':
            // 匹配预处理输出的行
            do {
                ch = getNextChar();
            } while (ch != '\n');
            ignore = true;
            return {};
        default:
            haveError = true;
            Logger::error("invalid character of `" + std::string(1, ch) + "`");
            rollbackLastChar();
            return {};
    }
}

std::vector<Token> *Lexer::analysis() {
    auto *tokenList = new std::vector<Token>();
    while (true) {
        bool ignore = false;
        Token token = analysisNextToken(ignore);
        if (haveError) {
            return {};
        }
        if (!ignore) {
            tokenList->push_back(token);
            if (token.id == TokenId::SPECIAL_EOF) {
                return tokenList;
            }
        }
    }
}

bool Lexer::isHaveError() const {
    return haveError;
}




