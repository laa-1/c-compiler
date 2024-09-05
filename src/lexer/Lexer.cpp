#include "Lexer.h"

#include <string>
#include <set>
#include "../error/ErrorHandler.h"

Lexer::Lexer(std::vector<std::vector<char>> *charLineList) : charLineList(charLineList) {}

inline bool Lexer::characterIsDigit() const {
    return character >= 48 && character <= 57;
}

inline bool Lexer::characterDigitOrLetterOrUnderscore() const {
    return (character >= 48 && character <= 57) || (character >= 65 && character <= 90) || (character >= 97 && character <= 122) || character == 95;
}

inline bool Lexer::bufferIsKeyword() {
    return keywordSet.contains(buffer);
}

inline void Lexer::nextCharacter() {
    if ((*charLineList)[lineIndex].empty() || columnIndex == (*charLineList)[lineIndex].size() - 1) {
        lineIndex++;
        columnIndex = 0;
        while ((*charLineList)[lineIndex].empty()) {
            lineIndex++;
        }
    } else {
        columnIndex++;
    }
    character = (*charLineList)[lineIndex][columnIndex];
}

inline void Lexer::rollbackCharacter() {
    if (columnIndex == 0) {
        lineIndex--;
        columnIndex = static_cast<int>((*charLineList)[lineIndex].size()) - 1;
    } else {
        columnIndex--;
    }
    character = (*charLineList)[lineIndex][columnIndex];
}

Token Lexer::analysisNextToken(bool &needIgnore) {
    buffer = "";
    nextCharacter();
    int lineNumber = lineIndex + 1;
    int columnNumber = columnIndex + 1;
    switch (character) {
        case EOF:
            return {TokenId::SPECIAL_EOF, "", lineNumber, columnNumber};
        case ' ':
        case '\r':
        case '\t':
            needIgnore = true;
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
                buffer += character;
                nextCharacter();
            } while (characterDigitOrLetterOrUnderscore());
            rollbackCharacter();
            if (bufferIsKeyword()) {
                // 关键字
                return {keywordTokenIdMap[buffer], buffer, lineNumber, columnNumber};
            } else {
                // 标识符
                return {TokenId::IDENTIFIER, buffer, lineNumber, columnNumber};
            }
        case '0':
            // 匹配0开头的数值常量
            buffer += character;
            nextCharacter();
            if (character == '.') {
                // 匹配浮点数常量
                buffer += character;
                nextCharacter();
                if (!characterIsDigit()) {
                    rollbackCharacter();
                    ErrorHandler::error(lineNumber, columnNumber, "numerical constant cannot end with a dot");
                    return {};
                }
                while (characterIsDigit()) {
                    buffer += character;
                    nextCharacter();
                }
                rollbackCharacter();
                return {TokenId::LITERAL_FLOATING_POINT, buffer, lineNumber, columnNumber};
            } else {
                // 数值0
                rollbackCharacter();
                return {TokenId::LITERAL_INTEGER, buffer, lineNumber, columnNumber};
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
            while (characterIsDigit()) {
                buffer += character;
                nextCharacter();
            }
            if (character == '.') {
                // 匹配浮点数常量
                buffer += character;
                nextCharacter();
                if (!characterIsDigit()) {
                    rollbackCharacter();
                    ErrorHandler::error(lineNumber, columnNumber, "numerical constant cannot end with a dot");
                    return {};
                } else {
                    while (characterIsDigit()) {
                        buffer += character;
                        nextCharacter();
                    }
                    rollbackCharacter();
                    return {TokenId::LITERAL_FLOATING_POINT, buffer, lineNumber, columnNumber};
                }
            } else {
                // 整形常量
                rollbackCharacter();
                return {TokenId::LITERAL_INTEGER, buffer, lineNumber, columnNumber};
            }
        case '\'':
            // 匹配字符常量
            nextCharacter();
            if (character == '\\') {
                // 匹配转义字符
                nextCharacter();
                switch (character) {
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
                        buffer += character;
                        break;
                }
            } else if (character == '\'') {
                ErrorHandler::error(lineNumber, columnNumber, "character constants are not allowed to have more than one character");
                return {};
            } else {
                buffer += character;
            }
            nextCharacter();
            if (character != '\'') {
                rollbackCharacter();
                ErrorHandler::error(lineNumber, columnNumber, "character constants are not allowed to have more than one character");
                return {};
            }
            return {TokenId::LITERAL_CHARACTER, buffer};
        case '"':
            // 匹配字符串字面量
            while (true) {
                nextCharacter();
                if (character == '\\') {
                    // 匹配转义字符
                    nextCharacter();
                    switch (character) {
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
                            buffer += character;
                            break;
                    }
                } else if (character == '"') {
                    return {TokenId::LITERAL_STRING, buffer, lineNumber, columnNumber};
                } else if (character == EOF) {
                    ErrorHandler::error(lineNumber, columnNumber, "string constants cannot be used without the right double quote");
                    return {};
                } else {
                    buffer += character;
                }
            }
        case '+':
            nextCharacter();
            if (character == '+') {
                return {TokenId::PUNCTUATOR_INCREMENT, "++", lineNumber, columnNumber};
            } else if (character == '=') {
                return {TokenId::PUNCTUATOR_ADD_ASSIGN, "+=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_ADD, "+", lineNumber, columnNumber};
            }
        case '-':
            nextCharacter();
            if (character == '-') {
                return {TokenId::PUNCTUATOR_DECREMENT, "--", lineNumber, columnNumber};
            } else if (character == '=') {
                return {TokenId::PUNCTUATOR_SUB_ASSIGN, "-=", lineNumber, columnNumber};
            } else if (character == '>') {
                return {TokenId::PUNCTUATOR_POINT_TO, "->", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_SUB, "-", lineNumber, columnNumber};
            }
        case '*':
            nextCharacter();
            if (character == '=') {
                return {TokenId::PUNCTUATOR_MUL_ASSIGN, "*=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_MUL, "*", lineNumber, columnNumber};
            }
        case '/':
            nextCharacter();
            if (character == '=') {
                return {TokenId::PUNCTUATOR_DIV_ASSIGN, "/=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_DIV, "/", lineNumber, columnNumber};
            }
        case '%':
            nextCharacter();
            if (character == '=') {
                return {TokenId::PUNCTUATOR_MOD_ASSIGN, "%=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_MOD, "%", lineNumber, columnNumber};
            }
        case '=':
            nextCharacter();
            if (character == '=') {
                return {TokenId::PUNCTUATOR_EQUAL, "==", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_ASSIGN, "=", lineNumber, columnNumber};
            }
        case '>':
            nextCharacter();
            if (character == '>') {
                nextCharacter();
                if (character == '=') {
                    return {TokenId::PUNCTUATOR_RIGHT_SHIFT_ASSIGN, ">>=", lineNumber, columnNumber};
                } else {
                    rollbackCharacter();
                    return {TokenId::PUNCTUATOR_RIGHT_SHIFT, ">>", lineNumber, columnNumber};
                }
            } else if (character == '=') {
                return {TokenId::PUNCTUATOR_GREATER_EQUAL, ">=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_GREATER, ">", lineNumber, columnNumber};
            }
        case '<':
            nextCharacter();
            if (character == '<') {
                nextCharacter();
                if (character == '=') {
                    return {TokenId::PUNCTUATOR_LEFT_SHIFT_ASSIGN, "<<=", lineNumber, columnNumber};
                } else {
                    rollbackCharacter();
                    return {TokenId::PUNCTUATOR_LEFT_SHIFT, "<<", lineNumber, columnNumber};
                }
            } else if (character == '=') {
                return {TokenId::PUNCTUATOR_LESS_EQUAL, "<=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_LESS, "<", lineNumber, columnNumber};
            }
        case '!':
            nextCharacter();
            if (character == '=') {
                return {TokenId::PUNCTUATOR_LOGICAL_NOT_EQUAL, "!=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_LOGICAL_NOT, "!", lineNumber, columnNumber};
            }
        case '&':
            nextCharacter();
            if (character == '&') {
                return {TokenId::PUNCTUATOR_LOGICAL_AND, "&&", lineNumber, columnNumber};
            } else if (character == '=') {
                return {TokenId::PUNCTUATOR_BITWISE_AND_ASSIGN, "&=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_BITWISE_AND, "&", lineNumber, columnNumber};
            }
        case '|':
            nextCharacter();
            if (character == '|') {
                return {TokenId::PUNCTUATOR_LOGICAL_OR, "||", lineNumber, columnNumber};
            } else if (character == '=') {
                return {TokenId::PUNCTUATOR_BITWISE_OR_ASSIGN, "|=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_BITWISE_OR, "|", lineNumber, columnNumber};
            }
        case '^':
            nextCharacter();
            if (character == '=') {
                return {TokenId::PUNCTUATOR_EXCLUSIVE_OR_ASSIGN, "^=", lineNumber, columnNumber};
            } else {
                rollbackCharacter();
                return {TokenId::PUNCTUATOR_EXCLUSIVE_OR, "^", lineNumber, columnNumber};
            }
        case '~':
            return {TokenId::PUNCTUATOR_BITWISE_NOT, "~", lineNumber, columnNumber};
        case '?':
            return {TokenId::PUNCTUATOR_QUESTION, "?", lineNumber, columnNumber};
        case ':':
            return {TokenId::PUNCTUATOR_COLON, ":", lineNumber, columnNumber};
        case '(':
            return {TokenId::PUNCTUATOR_LEFT_PARENTHESES, "(", lineNumber, columnNumber};
        case ')':
            return {TokenId::PUNCTUATOR_RIGHT_PARENTHESES, ")", lineNumber, columnNumber};
        case '[':
            return {TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS, "[", lineNumber, columnNumber};
        case ']':
            return {TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS, "]", lineNumber, columnNumber};
        case '{':
            return {TokenId::PUNCTUATOR_LEFT_CURLY_BRACES, "{", lineNumber, columnNumber};
        case '}':
            return {TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES, "}", lineNumber, columnNumber};
        case ',':
            return {TokenId::PUNCTUATOR_COMMA, ",", lineNumber, columnNumber};
        case ';':
            return {TokenId::PUNCTUATOR_SEMICOLON, ";", lineNumber, columnNumber};
        case '.':
            return {TokenId::PUNCTUATOR_DOT, ".", lineNumber, columnNumber};
        default:
            ErrorHandler::error(lineNumber, columnNumber, "invalid character of `" + std::string(1, character) + "`");
            rollbackCharacter();
            return {};
    }
}

void Lexer::analysis() {
    while (true) {
        bool needIgnore = false;
        Token token = analysisNextToken(needIgnore);
        if (ErrorHandler::getStatus()) {
            return;
        }
        if (!needIgnore) {
            tokenList->push_back(token);
            if (token.id == TokenId::SPECIAL_EOF) {
                return;
            }
        }
    }
}

std::vector<Token> *Lexer::analysis(std::vector<std::vector<char>> *charLineList) {
    std::unique_ptr<Lexer> lexer = std::unique_ptr<Lexer>(new Lexer(charLineList));
    lexer->analysis();
    return lexer->tokenList;
}




