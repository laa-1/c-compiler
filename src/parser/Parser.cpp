#include "Parser.h"

#include <algorithm>
#include <cassert>
#include "../ast/node/expression/BinaryExpression.h"
#include "../ast/node/expression/CallExpression.h"
#include "../ast/node/expression/CastExpression.h"
#include "../ast/node/expression/CharacterLiteralExpression.h"
#include "../ast/node/expression/FloatingPointLiteralExpression.h"
#include "../ast/node/expression/IdentifierExpression.h"
#include "../ast/node/expression/IntegerLiteralExpression.h"
#include "../ast/node/expression/StringLiteralExpression.h"
#include "../ast/node/expression/TernaryExpression.h"
#include "../ast/node/expression/UnaryExpression.h"
#include "../ast/node/type/ArrayType.h"
#include "../ast/node/type/ScalarType.h"
#include "../ast/node/type/FunctionType.h"
#include "../ast/node/type/PointerType.h"
#include "../ast/node/declaration/FunctionDeclaration.h"
#include "../ast/node/declaration/FunctionDefinition.h"
#include "../ast/node/declaration/VariableDeclaration.h"
#include "../ast/node/statement/BreakStatement.h"
#include "../ast/node/statement/CaseStatement.h"
#include "../ast/node/statement/CompoundStatement.h"
#include "../ast/node/statement/ContinueStatement.h"
#include "../ast/node/statement/DeclarationStatement.h"
#include "../ast/node/statement/DefaultStatement.h"
#include "../ast/node/statement/DoWhileStatement.h"
#include "../ast/node/statement/ExpressionStatement.h"
#include "../ast/node/statement/ForStatement.h"
#include "../ast/node/statement/GotoStatement.h"
#include "../ast/node/statement/IfStatement.h"
#include "../ast/node/statement/LabelStatement.h"
#include "../ast/node/statement/ReturnStatement.h"
#include "../ast/node/statement/SwitchStatement.h"
#include "../ast/node/statement/WhileStatement.h"
#include "../error/ErrorHandler.h"


template<typename T>
void clearAllElem(std::vector<T> &vec) {
    vec.clear();
}

template<typename T>
void clearAllElem(std::stack<T> &stk) {
    while (!stk.empty()) {
        stk.pop();
    }
}

template<typename T>
void deleteAndClearAllElem(std::vector<T> &vec) {
    for (auto e : vec) {
        delete e;
    }
    vec.clear();
}

template<typename T>
void deleteAndClearAllElem(std::stack<T> &stk) {
    while (!stk.empty()) {
        delete stk.top();
        stk.pop();
    }
}

bool haveEmptyString(const std::vector<std::string> &strList) {
    return std::any_of(strList.begin(), strList.end(), [](const std::string &str) {
        return str.empty();
    });
}

Type *typeStack2Type(int lineNumber, int columnNumber, const std::vector<TypeQualifier> &typeQualifierList, BaseType typeSpecifier, std::stack<Type *> typeStack) {
    Type *finalType = new ScalarType(lineNumber, columnNumber, typeSpecifier, typeQualifierList);
    while (!typeStack.empty()) {
        switch (typeStack.top()->getClass()) {
            case TypeClass::ARRAY_TYPE: {
                auto *newType = reinterpret_cast<ArrayType *>(typeStack.top());
                newType->elemType = finalType;
                finalType = newType;
                break;
            }
            case TypeClass::FUNCTION_TYPE: {
                auto *newType = reinterpret_cast<FunctionType *>(typeStack.top());
                newType->returnType = finalType;
                finalType = newType;
                break;
            }
            case TypeClass::POINTER_TYPE: {
                auto *newType = reinterpret_cast<PointerType *>(typeStack.top());
                newType->sourceType = finalType;
                finalType = newType;
                break;
            }
            case TypeClass::SCALAR_TYPE:
                assert(false);
        }
        typeStack.pop();
    }
    return finalType;
}

Parser::Parser(std::vector<Token> *tokenList) : tokenList(tokenList) {}

inline void Parser::nextToken() {
    currentIndex++;
    token = (*tokenList)[currentIndex];
    if (farthestIndex < currentIndex) {
        farthestIndex = currentIndex;
    }
}

inline void Parser::rollbackToken(int index) {
    currentIndex = index;
    token = (*tokenList)[currentIndex];
}

Expression *Parser::parsePrimaryExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::IDENTIFIER) {
        Token identifierToken = token;
        nextToken();
        return new IdentifierExpression(lineNumber, columnNumber, identifierToken.value);
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::LITERAL_INTEGER) {
        Token valueToken = token;
        nextToken();
        return new IntegerLiteralExpression(lineNumber, columnNumber, std::stoi(valueToken.value));
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::LITERAL_FLOATING_POINT) {
        Token valueToken = token;
        nextToken();
        return new FloatingPointLiteralExpression(lineNumber, columnNumber, std::stod(valueToken.value));
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::LITERAL_CHARACTER) {
        Token valueToken = token;
        nextToken();
        return new CharacterLiteralExpression(lineNumber, columnNumber, valueToken.value[0]);
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::LITERAL_STRING) {
        Token valueToken = token;
        nextToken();
        return new StringLiteralExpression(lineNumber, columnNumber, valueToken.value);
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        nextToken();
        Expression *expression = parseCommaExpression();
        if (expression != nullptr) {
            if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                nextToken();
                return expression;
            }
        }
        delete expression;
    }
    rollbackToken(tagIndex1);
    return nullptr;
}

Expression *Parser::parsePostfixExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    // 匹配左侧部分
    Expression *leftOperand = parsePrimaryExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    // 循环匹配右侧部分
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
            nextToken();
            Expression *rightOperand = parseCommaExpression();
            if (rightOperand != nullptr) {
                if (token.id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                    nextToken();
                    // 把结果作为新的左侧部分
                    leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::SUBSCRIPT, leftOperand, rightOperand);
                    continue;
                }
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            std::vector<Expression *> argumentList = parseAssignmentExpressionList();
            if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                nextToken();
                leftOperand = new CallExpression(lineNumber, columnNumber, leftOperand, argumentList);
                continue;
            }
            deleteAndClearAllElem(argumentList);
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_INCREMENT) {
            nextToken();
            leftOperand = new UnaryExpression(lineNumber, columnNumber, UnaryOperator::POSTINCREMENT, leftOperand);
            continue;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_DECREMENT) {
            nextToken();
            leftOperand = new UnaryExpression(lineNumber, columnNumber, UnaryOperator::POSTDECREMENT, leftOperand);
            continue;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseUnaryExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_INCREMENT) {
        nextToken();
        Expression *operand = parseUnaryExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::PREINCREMENT, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_DECREMENT) {
        nextToken();
        Expression *operand = parseUnaryExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::PREINCREMENT, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_BITWISE_AND) {
        nextToken();
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::TAKE_ADDRESS, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_MUL) {
        nextToken();
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::DEREFERENCE, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_ADD) {
        nextToken();
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::PLUS, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_SUB) {
        nextToken();
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::MINUS, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_BITWISE_NOT) {
        nextToken();
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::BITWISE_NOT, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_LOGICAL_NOT) {
        nextToken();
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::LOGICAL_NOT, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_SIZEOF) {
        nextToken();
        Expression *operand = parseUnaryExpression();
        if (operand != nullptr) {
            return new UnaryExpression(lineNumber, columnNumber, UnaryOperator::SIZEOF, operand);
        }
        delete operand;
    }
    rollbackToken(tagIndex1);
    Expression *postfixExpression = parsePostfixExpression();
    if (postfixExpression != nullptr) {
        return postfixExpression;
    }
    delete postfixExpression;
    rollbackToken(tagIndex1);
    return nullptr;
}

Expression *Parser::parseCastExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        nextToken();
        Type *typeName = parseTypeName();
        if (typeName != nullptr) {
            if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                nextToken();
                Expression *operand = parseCastExpression();
                if (operand != nullptr) {
                    return new CastExpression(lineNumber, columnNumber, typeName, operand);
                }
                delete operand;
            }
        }
        delete typeName;
    }
    rollbackToken(tagIndex1);
    Expression *unaryExpression = parseUnaryExpression();
    if (unaryExpression != nullptr) {
        return unaryExpression;
    }
    delete unaryExpression;
    rollbackToken(tagIndex1);
    return nullptr;
}

Expression *Parser::parseMultiplicativeExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseCastExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_MUL) {
            nextToken();
            Expression *rightOperand = parseCastExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::MUL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_DIV) {
            nextToken();
            Expression *rightOperand = parseCastExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::DIV, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_MOD) {
            nextToken();
            Expression *rightOperand = parseCastExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::MOD, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseAdditiveExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseMultiplicativeExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_ADD) {
            nextToken();
            Expression *rightOperand = parseMultiplicativeExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::ADD, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_SUB) {
            nextToken();
            Expression *rightOperand = parseMultiplicativeExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::SUB, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseShiftExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseAdditiveExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_LEFT_SHIFT) {
            nextToken();
            Expression *rightOperand = parseAdditiveExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::SHIFT_LEFT, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_RIGHT_SHIFT) {
            nextToken();
            Expression *rightOperand = parseAdditiveExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::SHIFT_RIGHT, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseRelationalExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseShiftExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_LESS) {
            nextToken();
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::LESS, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_GREATER) {
            nextToken();
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::GREATER, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_LESS_EQUAL) {
            nextToken();
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::LESS_EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_GREATER_EQUAL) {
            nextToken();
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::GREATER_EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseEqualityExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseRelationalExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_EQUAL) {
            nextToken();
            Expression *rightOperand = parseRelationalExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        if (token.id == TokenId::PUNCTUATOR_LOGICAL_NOT_EQUAL) {
            nextToken();
            Expression *rightOperand = parseRelationalExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::NOT_EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseAndExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseEqualityExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_BITWISE_AND) {
            nextToken();
            Expression *rightOperand = parseEqualityExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::BITWISE_AND, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseExclusiveOrExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseAndExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_EXCLUSIVE_OR) {
            nextToken();
            Expression *rightOperand = parseAndExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::BITWISE_XOR, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseInclusiveOrExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseExclusiveOrExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_BITWISE_OR) {
            nextToken();
            Expression *rightOperand = parseExclusiveOrExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::BITWISE_OR, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseLogicalAndExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseInclusiveOrExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_LOGICAL_AND) {
            nextToken();
            Expression *rightOperand = parseInclusiveOrExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::LOGICAL_AND, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseLogicalOrExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseLogicalAndExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_LOGICAL_OR) {
            nextToken();
            Expression *rightOperand = parseLogicalAndExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::LOGICAL_OR, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

Expression *Parser::parseConditionalExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseLogicalOrExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_QUESTION) {
        nextToken();
        Expression *middleOperand = parseCommaExpression();
        if (middleOperand != nullptr) {
            if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
                nextToken();
                Expression *rightOperand = parseConditionalExpression();
                if (rightOperand != nullptr) {
                    return new TernaryExpression(lineNumber, columnNumber, TernaryOperator::CONDITION, leftOperand, middleOperand, rightOperand);
                }
                delete rightOperand;
            }
        }
        delete middleOperand;
    }
    rollbackToken(tagIndex1);
    return leftOperand;
}

Expression *Parser::parseAssignmentExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    Expression *leftOperand = parseUnaryExpression();
    if (leftOperand != nullptr) {
        int tagIndex2 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_MUL_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::MUL_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_DIV_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::DIV_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_MOD_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::MOD_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_ADD_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::ADD_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_SUB_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::SUB_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_LEFT_SHIFT_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::SHIFT_LEFT_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_RIGHT_SHIFT_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::SHIFT_RIGHT_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_BITWISE_AND_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::BITWISE_AND_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_EXCLUSIVE_OR_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::BITWISE_XOR_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_BITWISE_OR_ASSIGN) {
            nextToken();
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(lineNumber, columnNumber, BinaryOperator::BITWISE_OR_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
    }
    delete leftOperand;
    rollbackToken(tagIndex1);
    Expression *conditionalExpression = parseConditionalExpression();
    if (conditionalExpression != nullptr) {
        return conditionalExpression;
    }
    delete conditionalExpression;
    rollbackToken(tagIndex1);
    return nullptr;
}

Expression *Parser::parseCommaExpression() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    Expression *leftOperand = parseAssignmentExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_COMMA) {
            nextToken();
            Expression *rightOperand = parseCommaExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(lineNumber, columnNumber, BinaryOperator::COMMA, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        rollbackToken(tagIndex1);
        return leftOperand;
    }
}

std::vector<Expression *> Parser::parseAssignmentExpressionList() {
    std::vector<Expression *> assignmentExpressionList;
    Expression *assignmentExpression = parseAssignmentExpression();
    if (assignmentExpression == nullptr) {
        return {};
    }
    assignmentExpressionList.push_back(assignmentExpression);
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_COMMA) {
            nextToken();
            assignmentExpression = parseAssignmentExpression();
            if (assignmentExpression != nullptr) {
                assignmentExpressionList.push_back(assignmentExpression);
                continue;
            }
        }
        rollbackToken(tagIndex1);
        return assignmentExpressionList;
    }
}

std::vector<StorageSpecifier> Parser::parseStorageSpecifierList() {
    std::vector<StorageSpecifier> storageSpecifierList;
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::KEYWORD_TYPEDEF) {
            nextToken();
            storageSpecifierList.push_back(StorageSpecifier::TYPEDEF);
            continue;
        } else if (token.id == TokenId::KEYWORD_EXTERN) {
            nextToken();
            storageSpecifierList.push_back(StorageSpecifier::EXTERN);
            continue;
        } else if (token.id == TokenId::KEYWORD_STATIC) {
            nextToken();
            storageSpecifierList.push_back(StorageSpecifier::STATIC);
            continue;
        } else if (token.id == TokenId::KEYWORD_AUTO) {
            nextToken();
            storageSpecifierList.push_back(StorageSpecifier::AUTO);
            continue;
        } else if (token.id == TokenId::KEYWORD_REGISTER) {
            nextToken();
            storageSpecifierList.push_back(StorageSpecifier::REGISTER);
            continue;
        }
        rollbackToken(tagIndex1);
        return storageSpecifierList;
    }
}

std::vector<FunctionSpecifier> Parser::parseFunctionSpecifierList() {
    std::vector<FunctionSpecifier> functionSpecifierList;
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::KEYWORD_INLINE) {
            nextToken();
            functionSpecifierList.push_back(FunctionSpecifier::INLINE);
            continue;
        }
        rollbackToken(tagIndex1);
        return functionSpecifierList;
    }
}

std::vector<TypeQualifier> Parser::parseTypeQualifierList() {
    std::vector<TypeQualifier> typeQualifierList;
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::KEYWORD_CONST) {
            nextToken();
            typeQualifierList.push_back(TypeQualifier::CONST);
            continue;
        } else if (token.id == TokenId::KEYWORD_RESTRICT) {
            nextToken();
            typeQualifierList.push_back(TypeQualifier::RESTRICT);
            continue;
        } else if (token.id == TokenId::KEYWORD_VOLATILE) {
            nextToken();
            typeQualifierList.push_back(TypeQualifier::VOLATILE);
            continue;
        }
        rollbackToken(tagIndex1);
        return typeQualifierList;
    }
}

bool Parser::parseTypeSpecifier(BaseType &baseType) {
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::KEYWORD_VOID) {
        nextToken();
        baseType = BaseType::VOID;
        return true;
    } else if (token.id == TokenId::KEYWORD_CHAR) {
        nextToken();
        baseType = BaseType::CHAR;
        return true;
    } else if (token.id == TokenId::KEYWORD_SHORT) {
        nextToken();
        baseType = BaseType::SHORT;
        return true;
    } else if (token.id == TokenId::KEYWORD_INT) {
        nextToken();
        baseType = BaseType::INT;
        return true;
    } else if (token.id == TokenId::KEYWORD_LONG) {
        nextToken();
        int tagIndex2 = currentIndex;
        if (token.id == TokenId::KEYWORD_LONG) {
            nextToken();
            int tagIndex3 = currentIndex;
            if (token.id != TokenId::KEYWORD_INT) {
                nextToken();
                rollbackToken(tagIndex3);
            }
            baseType = BaseType::LONG_LONG_INT;
            return true;
        }
        rollbackToken(tagIndex2);
        baseType = BaseType::LONG_INT;
        return true;
    } else if (token.id == TokenId::KEYWORD_FLOAT) {
        nextToken();
        baseType = BaseType::FLOAT;
        return true;
    } else if (token.id == TokenId::KEYWORD_DOUBLE) {
        nextToken();
        baseType = BaseType::DOUBLE;
        return true;
    } else if (token.id == TokenId::KEYWORD_SIGNED) {
        nextToken();
        int tagIndex2 = currentIndex;
        if (token.id == TokenId::KEYWORD_CHAR) {
            nextToken();
            baseType = BaseType::CHAR;
            return true;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::KEYWORD_SHORT) {
            nextToken();
            baseType = BaseType::SHORT;
            return true;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::KEYWORD_INT) {
            nextToken();
            baseType = BaseType::INT;
            return true;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::KEYWORD_LONG) {
            nextToken();
            int tagIndex3 = currentIndex;
            if (token.id == TokenId::KEYWORD_LONG) {
                nextToken();
                int tagIndex4 = currentIndex;
                if (token.id != TokenId::KEYWORD_INT) {
                    nextToken();
                    currentIndex = tagIndex4;
                }
                baseType = BaseType::LONG_LONG_INT;
                return true;
            }
            rollbackToken(tagIndex3);
            baseType = BaseType::LONG_INT;
            return true;
        }
        return false;
    } else if (token.id == TokenId::KEYWORD_UNSIGNED) {
        nextToken();
        int tagIndex2 = currentIndex;
        if (token.id == TokenId::KEYWORD_CHAR) {
            nextToken();
            baseType = BaseType::UNSIGNED_CHAR;
            return true;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::KEYWORD_SHORT) {
            nextToken();
            baseType = BaseType::UNSIGNED_SHORT;
            return true;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::KEYWORD_INT) {
            nextToken();
            baseType = BaseType::UNSIGNED_INT;
            return true;
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::KEYWORD_LONG) {
            nextToken();
            int tagIndex3 = currentIndex;
            if (token.id == TokenId::KEYWORD_LONG) {
                nextToken();
                int tagIndex4 = currentIndex;
                if (token.id != TokenId::KEYWORD_INT) {
                    nextToken();
                    currentIndex = tagIndex4;
                }
                baseType = BaseType::UNSIGNED_LONG_LONG_INT;
                return true;
            }
            rollbackToken(tagIndex3);
            baseType = BaseType::UNSIGNED_LONG_INT;
            return true;
        }
        return false;
    }
    rollbackToken(tagIndex1);
    return false;
}

std::vector<Expression *> Parser::parseInitializer() {
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_LEFT_CURLY_BRACES) {
        nextToken();
        std::vector<Expression *> initialValueList = parseAssignmentExpressionList();
        if (token.id == TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES) {
            nextToken();
            return initialValueList;
        }
        deleteAndClearAllElem(initialValueList);
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_LEFT_CURLY_BRACES) {
        nextToken();
        std::vector<Expression *> initialValueList = parseAssignmentExpressionList();
        if (token.id == TokenId::PUNCTUATOR_COMMA) {
            nextToken();
            if (token.id == TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES) {
                nextToken();
                return initialValueList;
            }
        }
        deleteAndClearAllElem(initialValueList);
    }
    rollbackToken(tagIndex1);
    Expression *initialValue = parseAssignmentExpression();
    if (initialValue != nullptr) {
        return {initialValue};
    }
    delete initialValue;
    rollbackToken(tagIndex1);
    return {};
}

bool Parser::parseDirectDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack) {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::IDENTIFIER) {
        Token identifierToken = token;
        nextToken();
        identifierList.push_back(identifierToken.value);
        goto whileParseDirectDeclaratorSuffix;
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        nextToken();
        if (parsePointerDeclarator(identifierList, typeStack)) {
            if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                nextToken();
                goto whileParseDirectDeclaratorSuffix;
            }
        }
    }
    rollbackToken(tagIndex1);
    return false;

    whileParseDirectDeclaratorSuffix:
    while (true) {
        int tagIndex2 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
            nextToken();
            int tagIndex3 = currentIndex;
            if (token.id == TokenId::LITERAL_INTEGER) {
                Token sizeToken = token;
                nextToken();
                if (token.id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                    nextToken();
                    typeStack.push(new ArrayType(lineNumber, columnNumber, nullptr, std::stoi(sizeToken.value)));
                    continue;
                }
            } else {
                rollbackToken(tagIndex3);
                if (token.id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                    nextToken();
                    typeStack.push(new ArrayType(lineNumber, columnNumber, nullptr, 0));
                    continue;
                }
            }
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            std::vector<Type *> parameterTypeList = parseParameterList(identifierList);
            if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                nextToken();
                typeStack.push(new FunctionType(lineNumber, columnNumber, nullptr, parameterTypeList));
                continue;
            }
            deleteAndClearAllElem(parameterTypeList);
        }
        rollbackToken(tagIndex2);
        return true;
    }
}

bool Parser::parsePointerDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack) {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_MUL) {
        nextToken();
        std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
        if (parsePointerDeclarator(identifierList, typeStack)) {
            typeStack.push(new PointerType(lineNumber, columnNumber, nullptr, typeQualifierList));
            return true;
        }
    }
    rollbackToken(tagIndex1);
    if (parseDirectDeclarator(identifierList, typeStack)) {
        return true;
    }
    rollbackToken(tagIndex1);
    return false;
}

bool Parser::parseDirectAbstractDeclarator(std::stack<Type *> &typeStack) {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        nextToken();
        if (parsePointerAbstractDeclarator(typeStack)) {
            if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                nextToken();
                goto whileParseDirectAbstractDeclaratorSuffix;
            }
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
        nextToken();
        int tagIndex2 = currentIndex;
        if (token.id != TokenId::LITERAL_INTEGER) {
            nextToken();
            rollbackToken(tagIndex2);
        }
        if (token.id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
            nextToken();
            typeStack.push(new ArrayType(lineNumber, columnNumber, nullptr, 0));
            goto whileParseDirectAbstractDeclaratorSuffix;
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        nextToken();
        std::vector<std::string> identifierList;
        std::vector<Type *> parameterTypeList = parseParameterList(identifierList);
        if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
            nextToken();
            typeStack.push(new FunctionType(lineNumber, columnNumber, nullptr, parameterTypeList));
            goto whileParseDirectAbstractDeclaratorSuffix;
        }
        deleteAndClearAllElem(parameterTypeList);
    }
    rollbackToken(tagIndex1);
    return false;

    whileParseDirectAbstractDeclaratorSuffix:
    while (true) {
        int tagIndex2 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
            nextToken();
            int tagIndex3 = currentIndex;
            if (token.id != TokenId::KEYWORD_INT) {
                nextToken();
                rollbackToken(tagIndex3);
            }
            if (token.id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                nextToken();
                typeStack.push(new ArrayType(lineNumber, columnNumber, nullptr, 0));
                continue;
            }
        }
        rollbackToken(tagIndex2);
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            std::vector<std::string> identifierList;
            std::vector<Type *> parameterTypeList = parseParameterList(identifierList);
            if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                nextToken();
                typeStack.push(new FunctionType(lineNumber, columnNumber, nullptr, parameterTypeList));
                continue;
            }
            deleteAndClearAllElem(parameterTypeList);
        }
        rollbackToken(tagIndex2);
        return true;
    }
}

bool Parser::parsePointerAbstractDeclarator(std::stack<Type *> &typeStack) {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_MUL) {
        nextToken();
        std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
        parsePointerAbstractDeclarator(typeStack);
        typeStack.push(new PointerType(lineNumber, columnNumber, nullptr, typeQualifierList));
        return true;
    }
    rollbackToken(tagIndex1);
    if (parseDirectAbstractDeclarator(typeStack)) {
        return true;
    }
    rollbackToken(tagIndex1);
    return false;
}

Type *Parser::parseParameter(std::vector<std::string> &identifierList) {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        rollbackToken(tagIndex1);
        return nullptr;
    }
    std::stack<Type *> typeStack;
    int tagIndex2 = currentIndex;
    if (parsePointerDeclarator(identifierList, typeStack)) {
        return typeStack2Type(lineNumber, columnNumber, typeQualifierList, typeSpecifier, typeStack);
    }
    deleteAndClearAllElem(typeStack);
    rollbackToken(tagIndex2);
    if (parsePointerAbstractDeclarator(typeStack)) {
        identifierList.emplace_back(""); // 抽象声明不需要标识符，但是这里需要进行占位，以便后续定位到参数的标识符
        return typeStack2Type(lineNumber, columnNumber, typeQualifierList, typeSpecifier, typeStack);
    }
    rollbackToken(tagIndex2);
    identifierList.emplace_back("");
    return new ScalarType(lineNumber, columnNumber, typeSpecifier, typeQualifierList);
}

std::vector<Type *> Parser::parseParameterList(std::vector<std::string> &identifierList) {
    std::vector<Type *> parameterTypeList;
    Type *parameterType = parseParameter(identifierList);
    if (parameterType == nullptr) {
        return {};
    }
    parameterTypeList.push_back(parameterType);
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_COMMA) {
            nextToken();
            parameterType = parseParameter(identifierList);
            if (parameterType != nullptr) {
                parameterTypeList.push_back(parameterType);
                continue;
            }
        }
        rollbackToken(tagIndex1);
        return parameterTypeList;
    }
}

bool Parser::parseInitDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack, std::vector<Expression *> &initialValueList) {
    int tagIndex1 = currentIndex;
    if (parsePointerDeclarator(identifierList, typeStack)) {
        if (token.id == TokenId::PUNCTUATOR_ASSIGN) {
            nextToken();
            initialValueList = parseInitializer();
            if (!initialValueList.empty()) {
                return true;
            }
        }
    }
    clearAllElem(identifierList);
    deleteAndClearAllElem(typeStack);
    deleteAndClearAllElem(initialValueList);
    rollbackToken(tagIndex1);
    if (parsePointerDeclarator(identifierList, typeStack)) {
        return true;
    }
    rollbackToken(tagIndex1);
    return false;
}

bool Parser::parseInitDeclaratorList(std::vector<std::vector<std::string>> &identifierListList, std::vector<std::stack<Type *>> &typeStackList, std::vector<std::vector<Expression *>> &initialValueListList) {
    std::vector<std::string> identifierList;
    std::stack<Type *> typeStack;
    std::vector<Expression *> initialValueList;
    if (!parseInitDeclarator(identifierList, typeStack, initialValueList)) {
        deleteAndClearAllElem(typeStack);
        deleteAndClearAllElem(initialValueList);
        return false;
    }
    identifierListList.push_back(identifierList);
    typeStackList.push_back(typeStack);
    initialValueListList.push_back(initialValueList);
    while (true) {
        int tagIndex1 = currentIndex;
        if (token.id == TokenId::PUNCTUATOR_COMMA) {
            nextToken();
            clearAllElem(identifierList);
            clearAllElem(typeStack);
            clearAllElem(initialValueList);
            if (parseInitDeclarator(identifierList, typeStack, initialValueList)) {
                identifierListList.push_back(identifierList);
                typeStackList.push_back(typeStack);
                initialValueListList.push_back(initialValueList);
                continue;
            }
            deleteAndClearAllElem(typeStack);
            deleteAndClearAllElem(initialValueList);
        }
        rollbackToken(tagIndex1);
        return true;
    }
}

std::vector<Declaration *> Parser::parseDeclaration() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    std::vector<FunctionSpecifier> functionSpecifierList = parseFunctionSpecifierList();
    std::vector<StorageSpecifier> storageSpecifierList = parseStorageSpecifierList();
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        rollbackToken(tagIndex1);
        return {};
    }
    std::vector<std::vector<std::string>> identifierListList;
    std::vector<std::stack<Type *>> typeStackList;
    std::vector<std::vector<Expression *>> initialValueListList;
    if (parseInitDeclaratorList(identifierListList, typeStackList, initialValueListList)) {
        if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
            nextToken();
            std::vector<Declaration *> declarationList;
            for (int i = 0; i < identifierListList.size(); i++) {
                Type *finalType = typeStack2Type(lineNumber, columnNumber, typeQualifierList, typeSpecifier, typeStackList[i]);
                switch (finalType->getClass()) {
                    case TypeClass::ARRAY_TYPE:
                        if (initialValueListList[i].size() == 1 && initialValueListList[i][0]->getClass() == ExpressionClass::STRING_LITERAL_EXPRESSION) {
                            std::string initialValue = reinterpret_cast<StringLiteralExpression *>(initialValueListList[i][0])->value;
                            initialValue.push_back('\0');
                            deleteAndClearAllElem(initialValueListList[i]);
                            for (char ch : initialValue) {
                                initialValueListList[i].push_back(new CharacterLiteralExpression(lineNumber, columnNumber, ch));
                            }
                        }
                        declarationList.push_back(new VariableDeclaration(lineNumber, columnNumber, storageSpecifierList, finalType, identifierListList[i][0], initialValueListList[i]));
                        break;
                    case TypeClass::FUNCTION_TYPE:
                        declarationList.push_back(new FunctionDeclaration(lineNumber, columnNumber, functionSpecifierList, finalType, identifierListList[i][0]));
                        deleteAndClearAllElem(initialValueListList[i]);
                        break;
                    case TypeClass::POINTER_TYPE:
                    case TypeClass::SCALAR_TYPE:
                        declarationList.push_back(new VariableDeclaration(lineNumber, columnNumber, storageSpecifierList, finalType, identifierListList[i][0], initialValueListList[i]));
                        break;
                }
            }
            return declarationList;
        }
    }
    for (auto typeStack : typeStackList) deleteAndClearAllElem(typeStack);
    for (auto initialValueList : initialValueListList) deleteAndClearAllElem(initialValueList);
    rollbackToken(tagIndex1);
    return {};
}

Type *Parser::parseTypeName() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        rollbackToken(tagIndex1);
        return nullptr;
    }
    int tagIndex2 = currentIndex;
    std::stack<Type *> typeStack;
    if (parsePointerAbstractDeclarator(typeStack)) {
        return typeStack2Type(lineNumber, columnNumber, typeQualifierList, typeSpecifier, typeStack);
    }
    deleteAndClearAllElem(typeStack);
    rollbackToken(tagIndex2);
    return new ScalarType(lineNumber, columnNumber, typeSpecifier, typeQualifierList);
}

Statement *Parser::parseBlockItem() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    std::vector<Declaration *> declarationList = parseDeclaration();
    if (!declarationList.empty()) {
        return new DeclarationStatement(lineNumber, columnNumber, declarationList);
    }
    deleteAndClearAllElem(declarationList);
    rollbackToken(tagIndex1);
    Statement *blockItem = parseStatement();
    if (blockItem != nullptr) {
        return blockItem;
    }
    delete blockItem;
    rollbackToken(tagIndex1);
    return nullptr;
}

std::vector<Statement *> Parser::parseBlockItemList() {
    std::vector<Statement *> blockItemList;
    Statement *statement;
    while ((statement = parseBlockItem()) != nullptr) {
        blockItemList.push_back(statement);
    }
    return blockItemList;
}

Statement *Parser::parseLabeledStatement() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::IDENTIFIER) {
        Token identifierToken = token;
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_COLON) {
            nextToken();
            Statement *statement = parseStatement();
            if (statement != nullptr) {
                return new LabelStatement(lineNumber, columnNumber, identifierToken.value, statement);
            }
            delete statement;
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_CASE) {
        nextToken();
        if (token.id == TokenId::KEYWORD_INT) {
            Token valueToken = token;
            nextToken();
            if (token.id == TokenId::PUNCTUATOR_COLON) {
                nextToken();
                Statement *statement = parseStatement();
                if (statement != nullptr) {
                    return new CaseStatement(lineNumber, columnNumber, std::stoi(valueToken.value), statement);
                }
            }
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_DEFAULT) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_COLON) {
            nextToken();
            Statement *statement = parseStatement();
            if (statement != nullptr) {
                return new DefaultStatement(lineNumber, columnNumber, statement);
            }
        }
    }
    rollbackToken(tagIndex1);
    return nullptr;
}

Statement *Parser::parseCompoundStatement() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::PUNCTUATOR_LEFT_CURLY_BRACES) {
        nextToken();
        std::vector<Statement *> statementList = parseBlockItemList();
        if (token.id == TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES) {
            nextToken();
            return new CompoundStatement(lineNumber, columnNumber, statementList);
        }
        deleteAndClearAllElem(statementList);
    }
    rollbackToken(tagIndex1);
    return nullptr;
}

Statement *Parser::parseExpressionStatement() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    Expression *expression = parseCommaExpression();
    if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
        nextToken();
        return new ExpressionStatement(lineNumber, columnNumber, expression);
    }
    delete expression;
    rollbackToken(tagIndex1);
    return nullptr;
}

Statement *Parser::parseSelectionStatement() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::KEYWORD_IF) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            Expression *condition = parseCommaExpression();
            if (condition != nullptr) {
                if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    nextToken();
                    Statement *trueBody = parseStatement();
                    if (trueBody != nullptr) {
                        if (token.id == TokenId::KEYWORD_ELSE) {
                            nextToken();
                            Statement *falseBody = parseStatement();
                            if (falseBody != nullptr) {
                                return new IfStatement(lineNumber, columnNumber, condition, trueBody, falseBody);
                            }
                            delete falseBody;
                        }
                    }
                    delete trueBody;
                }
            }
            delete condition;
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_IF) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            Expression *condition = parseCommaExpression();
            if (condition != nullptr) {
                if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    nextToken();
                    Statement *trueBody = parseStatement();
                    if (trueBody != nullptr) {
                        return new IfStatement(lineNumber, columnNumber, condition, trueBody, nullptr);
                    }
                    delete trueBody;
                }
            }
            delete condition;
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_SWITCH) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            Expression *expression = parseCommaExpression();
            if (expression != nullptr) {
                if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    nextToken();
                    Statement *body = parseStatement();
                    if (body != nullptr) {
                        return new SwitchStatement(lineNumber, columnNumber, expression, body);
                    }
                    delete body;
                }
            }
            delete expression;
        }
    }
    rollbackToken(tagIndex1);
    return nullptr;
}

Statement *Parser::parseIterationStatement() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::KEYWORD_WHILE) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            Expression *condition = parseCommaExpression();
            if (condition != nullptr) {
                if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    nextToken();
                    Statement *body = parseStatement();
                    if (body != nullptr) {
                        return new WhileStatement(lineNumber, columnNumber, condition, body);
                    }
                    delete body;
                }
            }
            delete condition;
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_DO) {
        nextToken();
        Statement *body = parseStatement();
        if (token.id == TokenId::KEYWORD_WHILE) {
            nextToken();
            if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
                nextToken();
                Expression *condition = parseCommaExpression();
                if (condition != nullptr) {
                    if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                        nextToken();
                        if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
                            nextToken();
                            return new DoWhileStatement(lineNumber, columnNumber, body, condition);
                        }
                    }
                }
                delete condition;
            }
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_FOR) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            Expression *initExpression = parseCommaExpression();
            if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
                nextToken();
                Expression *condition = parseCommaExpression();
                if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
                    nextToken();
                    Expression *updateExpression = parseCommaExpression();
                    if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                        nextToken();
                        Statement *body = parseStatement();
                        if (body != nullptr) {
                            return new ForStatement(lineNumber, columnNumber, {}, initExpression, condition, updateExpression, body);
                        }
                        delete body;
                    }
                    delete updateExpression;
                }
                delete condition;
            }
            delete initExpression;
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_FOR) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            nextToken();
            std::vector<Declaration *> initDeclarationList = parseDeclaration();
            Expression *condition = parseCommaExpression();
            if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
                nextToken();
                Expression *update = parseCommaExpression();
                if (token.id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    nextToken();
                    Statement *body = parseStatement();
                    if (body != nullptr) {
                        return new ForStatement(lineNumber, columnNumber, initDeclarationList, nullptr, condition, update, body);
                    }
                    delete body;
                }
                delete update;
            }
            delete condition;
            deleteAndClearAllElem(initDeclarationList);
        }
    }
    rollbackToken(tagIndex1);
    return nullptr;
}

Statement *Parser::parseJumpStatement() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    if (token.id == TokenId::KEYWORD_GOTO) {
        nextToken();
        if (token.id == TokenId::IDENTIFIER) {
            Token identifierToken = token;
            nextToken();
            if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
                nextToken();
                return new GotoStatement(lineNumber, columnNumber, identifierToken.value);
            }
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_CONTINUE) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
            nextToken();
            return new ContinueStatement(lineNumber, columnNumber);
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_BREAK) {
        nextToken();
        if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
            nextToken();
            return new BreakStatement(lineNumber, columnNumber);
        }
    }
    rollbackToken(tagIndex1);
    if (token.id == TokenId::KEYWORD_RETURN) {
        nextToken();
        Expression *value = parseCommaExpression();
        if (token.id == TokenId::PUNCTUATOR_SEMICOLON) {
            nextToken();
            return new ReturnStatement(lineNumber, columnNumber, value);
        }
        delete value;
    }
    rollbackToken(tagIndex1);
    return nullptr;
}

Statement *Parser::parseStatement() {
    Statement *statement;
    int tagIndex1 = currentIndex;
    statement = parseLabeledStatement();
    if (statement != nullptr) {
        return statement;
    }
    rollbackToken(tagIndex1);
    statement = parseCompoundStatement();
    if (statement != nullptr) {
        return statement;
    }
    rollbackToken(tagIndex1);
    statement = parseExpressionStatement();
    if (statement != nullptr) {
        return statement;
    }
    rollbackToken(tagIndex1);
    statement = parseSelectionStatement();
    if (statement != nullptr) {
        return statement;
    }
    rollbackToken(tagIndex1);
    statement = parseIterationStatement();
    if (statement != nullptr) {
        return statement;
    }
    rollbackToken(tagIndex1);
    statement = parseJumpStatement();
    if (statement != nullptr) {
        return statement;
    }
    rollbackToken(tagIndex1);
    return nullptr;
}

Declaration *Parser::parseFunctionDefinition() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    int tagIndex1 = currentIndex;
    std::vector<FunctionSpecifier> functionSpecifierList = parseFunctionSpecifierList();
    std::vector<StorageSpecifier> storageSpecifierList = parseStorageSpecifierList();
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        rollbackToken(tagIndex1);
        return {};
    }
    std::vector<std::string> identifierList;
    std::stack<Type *> typeStack;
    if (parsePointerDeclarator(identifierList, typeStack)) {
        Type *finalType = typeStack2Type(lineNumber, columnNumber, typeQualifierList, typeSpecifier, typeStack);
        if (finalType->getClass() == TypeClass::FUNCTION_TYPE) {
            std::vector<Type *> parameterTypeList = reinterpret_cast<FunctionType *>(finalType)->parameterTypeList;
            std::vector<std::string> parameterIdentifierList(identifierList.begin() + 1, identifierList.begin() + 1 + static_cast<int>(reinterpret_cast<FunctionType *>(finalType)->parameterTypeList.size()));
            if (parameterTypeList.size() == parameterIdentifierList.size()) {
                if (!haveEmptyString(parameterIdentifierList)) {
                    Statement *body = parseCompoundStatement();
                    if (body != nullptr) {
                        std::vector<Declaration *> parameterDeclarationList;
                        for (int i = 0; i < parameterTypeList.size(); i++) {
                            if (parameterTypeList[i]->getClass() == TypeClass::FUNCTION_TYPE) {
                                parameterDeclarationList.push_back(new FunctionDeclaration(lineNumber, columnNumber, {}, parameterTypeList[i]->clone(), parameterIdentifierList[i]));
                            } else {
                                parameterDeclarationList.push_back(new VariableDeclaration(lineNumber, columnNumber, {}, parameterTypeList[i]->clone(), parameterIdentifierList[i], {}));
                            }
                        }
                        return new FunctionDefinition(lineNumber, columnNumber, functionSpecifierList, finalType, identifierList[0], parameterDeclarationList, body);
                    }
                    delete body;
                }
            }
        }
        delete finalType;
    }
    rollbackToken(tagIndex1);
    return {};
}

std::vector<Declaration *> Parser::parseExternalDeclaration() {
    int tagIndex1 = currentIndex;
    Declaration *declaration = parseFunctionDefinition();
    if (declaration != nullptr) {
        return {declaration};
    }
    delete declaration;
    rollbackToken(tagIndex1);
    std::vector<Declaration *> declarationList = parseDeclaration();
    if (!declarationList.empty()) {
        return declarationList;
    }
    deleteAndClearAllElem(declarationList);
    rollbackToken(tagIndex1);
    return {};
}

TranslationUnit *Parser::parseTranslationUnit() {
    int lineNumber = token.lineNumber;
    int columnNumber = token.columnNumber;
    std::vector<Declaration *> declarationList;
    std::vector<Declaration *> newDeclarationList;
    while (!(newDeclarationList = parseExternalDeclaration()).empty()) {
        declarationList.insert(declarationList.end(), newDeclarationList.begin(), newDeclarationList.end());
    }
    return new TranslationUnit(lineNumber, columnNumber, declarationList);
}

void Parser::analysis() {
    token = (*tokenList)[currentIndex];
    translationUnit = parseTranslationUnit();
    if (farthestIndex != tokenList->size() - 1) {
        Token farthestToken = (*tokenList)[farthestIndex];
        ErrorHandler::error(farthestToken.lineNumber, farthestToken.columnNumber, "syntax error");
    }
}

TranslationUnit *Parser::analysis(std::vector<Token> *tokenList) {
    std::unique_ptr<Parser> parser = std::unique_ptr<Parser>(new Parser(tokenList));
    parser->analysis();
    return parser->translationUnit;
}


