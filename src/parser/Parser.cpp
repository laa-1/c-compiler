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
#include "../logger/Logger.h"

template<typename T>
void deleteAllElem(std::vector<T> &vec) {
    for (auto e : vec) {
        delete e;
    }
}

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

Type *typeStack2Type(const std::vector<TypeQualifier> &typeQualifierList, BaseType typeSpecifier, std::stack<Type *> typeStack) {
    Type *finalType = new ScalarType(typeSpecifier, typeQualifierList);
    while (!typeStack.empty()) {
        switch (typeStack.top()->getClass()) {
            case TypeClass::ARRAY_TYPE: {
                auto *newType = (ArrayType *) typeStack.top();
                newType->elemType = finalType;
                finalType = newType;
                break;
            }
            case TypeClass::FUNCTION_TYPE: {
                auto *newType = (FunctionType *) typeStack.top();
                newType->returnType = finalType;
                finalType = newType;
                break;
            }
            case TypeClass::POINTER_TYPE: {
                auto *newType = (PointerType *) typeStack.top();
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

Token Parser::getNextToken() {
    if (farthestIndex < nextIndex) {
        farthestIndex = nextIndex;
    }
    return (*tokenList)[nextIndex++];
}

Expression *Parser::parsePrimaryExpression() {
    int tagIndex1 = nextIndex;
    Token token = getNextToken();
    if (token.id == TokenId::IDENTIFIER) {
        return new IdentifierExpression(token.value);
    }
    nextIndex = tagIndex1;
    token = getNextToken();
    if (token.id == TokenId::LITERAL_INTEGER) {
        return new IntegerLiteralExpression(std::stoi(token.value));
    }
    nextIndex = tagIndex1;
    token = getNextToken();
    if (token.id == TokenId::LITERAL_FLOATING_POINT) {
        return new FloatingPointLiteralExpression(std::stod(token.value));
    }
    nextIndex = tagIndex1;
    token = getNextToken();
    if (token.id == TokenId::LITERAL_CHARACTER) {
        return new CharacterLiteralExpression(token.value[0]);
    }
    nextIndex = tagIndex1;
    token = getNextToken();
    if (token.id == TokenId::LITERAL_STRING) {
        return new StringLiteralExpression(token.value);
    }
    nextIndex = tagIndex1;
    token = getNextToken();
    if (token.id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        Expression *expression = parseCommaExpression();
        if (expression != nullptr) {
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                return expression;
            }
        }
        delete expression;
    }
    nextIndex = tagIndex1;
    return nullptr;
}

Expression *Parser::parsePostfixExpression() {
    // 匹配左侧部分
    Expression *leftOperand = parsePrimaryExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    // 循环匹配右侧部分
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
            Expression *rightOperand = parseCommaExpression();
            if (rightOperand != nullptr) {
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                    // 把结果作为新的左侧部分
                    leftOperand = new BinaryExpression(BinaryOperator::SUBSCRIPT, leftOperand, rightOperand);
                    continue;
                }
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            std::vector<Expression *> argumentList = parseAssignmentExpressionList();
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                leftOperand = new CallExpression(leftOperand, argumentList);
                continue;
            }
            deleteAllElem(argumentList);
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_INCREMENT) {
            leftOperand = new UnaryExpression(UnaryOperator::INCREMENT, leftOperand);
            continue;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_DECREMENT) {
            leftOperand = new UnaryExpression(UnaryOperator::DECREMENT, leftOperand);
            continue;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseUnaryExpression() {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_INCREMENT) {
        Expression *operand = parseUnaryExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::INCREMENT, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_DECREMENT) {
        Expression *operand = parseUnaryExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::INCREMENT, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_BITWISE_AND) {
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::TAKE_ADDRESS, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_MUL) {
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::DEREFERENCE, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_ADD) {
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::PLUS, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_SUB) {
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::MINUS, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_BITWISE_NOT) {
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::BITWISE_NOT, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_LOGICAL_NOT) {
        Expression *operand = parseCastExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::LOGICAL_NOT, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_SIZEOF) {
        Expression *operand = parseUnaryExpression();
        if (operand != nullptr) {
            return new UnaryExpression(UnaryOperator::SIZEOF, operand);
        }
        delete operand;
    }
    nextIndex = tagIndex1;
    Expression *postfixExpression = parsePostfixExpression();
    if (postfixExpression != nullptr) {
        return postfixExpression;
    }
    delete postfixExpression;
    nextIndex = tagIndex1;
    return nullptr;
}

Expression *Parser::parseCastExpression() {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        Type *typeName = parseTypeName();
        if (typeName != nullptr) {
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                Expression *operand = parseCastExpression();
                if (operand != nullptr) {
                    return new CastExpression(typeName, operand);
                }
                delete operand;
            }
        }
        delete typeName;
    }
    nextIndex = tagIndex1;
    Expression *unaryExpression = parseUnaryExpression();
    if (unaryExpression != nullptr) {
        return unaryExpression;
    }
    delete unaryExpression;
    nextIndex = tagIndex1;
    return nullptr;
}

Expression *Parser::parseMultiplicativeExpression() {
    Expression *leftOperand = parseCastExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_MUL) {
            Expression *rightOperand = parseCastExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::MUL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_DIV) {
            Expression *rightOperand = parseCastExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::DIV, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_MOD) {
            Expression *rightOperand = parseCastExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::MOD, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseAdditiveExpression() {
    Expression *leftOperand = parseMultiplicativeExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_ADD) {
            Expression *rightOperand = parseMultiplicativeExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::ADD, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_SUB) {
            Expression *rightOperand = parseMultiplicativeExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::SUB, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseShiftExpression() {
    Expression *leftOperand = parseAdditiveExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_SHIFT) {
            Expression *rightOperand = parseAdditiveExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::SHIFT_LEFT, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_SHIFT) {
            Expression *rightOperand = parseAdditiveExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::SHIFT_RIGHT, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseRelationalExpression() {
    Expression *leftOperand = parseShiftExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_LESS) {
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::LESS, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_GREATER) {
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::GREATER, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_LESS_EQUAL) {
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::LESS_EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_GREATER_EQUAL) {
            Expression *rightOperand = parseShiftExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::GREATER_EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseEqualityExpression() {
    Expression *leftOperand = parseRelationalExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_EQUAL) {
            Expression *rightOperand = parseRelationalExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        if (getNextToken().id == TokenId::PUNCTUATOR_LOGICAL_NOT_EQUAL) {
            Expression *rightOperand = parseRelationalExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::NOT_EQUAL, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseAndExpression() {
    Expression *leftOperand = parseEqualityExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_BITWISE_AND) {
            Expression *rightOperand = parseEqualityExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::BITWISE_AND, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseExclusiveOrExpression() {
    Expression *leftOperand = parseAndExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_EXCLUSIVE_OR) {
            Expression *rightOperand = parseAndExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::BITWISE_XOR, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseInclusiveOrExpression() {
    Expression *leftOperand = parseExclusiveOrExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_BITWISE_OR) {
            Expression *rightOperand = parseExclusiveOrExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::BITWISE_OR, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseLogicalAndExpression() {
    Expression *leftOperand = parseInclusiveOrExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_LOGICAL_AND) {
            Expression *rightOperand = parseInclusiveOrExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::LOGICAL_AND, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseLogicalOrExpression() {
    Expression *leftOperand = parseLogicalAndExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_LOGICAL_OR) {
            Expression *rightOperand = parseLogicalAndExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::LOGICAL_OR, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
        return leftOperand;
    }
}

Expression *Parser::parseConditionalExpression() {
    Expression *leftOperand = parseLogicalOrExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_QUESTION) {
        Expression *middleOperand = parseCommaExpression();
        if (middleOperand != nullptr) {
            if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
                Expression *rightOperand = parseConditionalExpression();
                if (rightOperand != nullptr) {
                    return new TernaryExpression(TernaryOperator::CONDITION, leftOperand, middleOperand, rightOperand);
                }
                delete rightOperand;
            }
        }
        delete middleOperand;
    }
    nextIndex = tagIndex1;
    return leftOperand;
}

Expression *Parser::parseAssignmentExpression() {
    int tagIndex1 = nextIndex;
    Expression *leftOperand = parseUnaryExpression();
    if (leftOperand != nullptr) {
        int tagIndex2 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_MUL_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::MUL_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_DIV_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::DIV_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_MOD_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::MOD_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_ADD_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::ADD_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_SUB_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::SUB_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_SHIFT_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::SHIFT_LEFT_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_SHIFT_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::SHIFT_RIGHT_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_BITWISE_AND_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::BITWISE_AND_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_EXCLUSIVE_OR_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::BITWISE_XOR_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_BITWISE_OR_ASSIGN) {
            Expression *rightOperand = parseAssignmentExpression();
            if (rightOperand != nullptr) {
                return new BinaryExpression(BinaryOperator::BITWISE_OR_ASSIGN, leftOperand, rightOperand);
            }
            delete rightOperand;
        }
    }
    delete leftOperand;
    nextIndex = tagIndex1;
    Expression *conditionalExpression = parseConditionalExpression();
    if (conditionalExpression != nullptr) {
        return conditionalExpression;
    }
    delete conditionalExpression;
    nextIndex = tagIndex1;
    return nullptr;
}

Expression *Parser::parseCommaExpression() {
    Expression *leftOperand = parseAssignmentExpression();
    if (leftOperand == nullptr) {
        return nullptr;
    }
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_COMMA) {
            Expression *rightOperand = parseCommaExpression();
            if (rightOperand != nullptr) {
                leftOperand = new BinaryExpression(BinaryOperator::COMMA, leftOperand, rightOperand);
                continue;
            }
            delete rightOperand;
        }
        nextIndex = tagIndex1;
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
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_COMMA) {
            assignmentExpression = parseAssignmentExpression();
            if (assignmentExpression != nullptr) {
                assignmentExpressionList.push_back(assignmentExpression);
                continue;
            }
        }
        nextIndex = tagIndex1;
        return assignmentExpressionList;
    }
}

std::vector<StorageSpecifier> Parser::parseStorageSpecifierList() {
    std::vector<StorageSpecifier> storageSpecifierList;
    while (true) {
        int tagIndex1 = nextIndex;
        Token token = getNextToken();
        if (token.id == TokenId::KEYWORD_EXTERN) {
            storageSpecifierList.push_back(StorageSpecifier::EXTERN);
            continue;
        } else if (token.id == TokenId::KEYWORD_STATIC) {
            storageSpecifierList.push_back(StorageSpecifier::STATIC);
            continue;
        }
        nextIndex = tagIndex1;
        return storageSpecifierList;
    }
}

std::vector<FunctionSpecifier> Parser::parseFunctionSpecifierList() {
    std::vector<FunctionSpecifier> functionSpecifierList;
    while (true) {
        int tagIndex1 = nextIndex;
        Token token = getNextToken();
        if (token.id == TokenId::KEYWORD_INLINE) {
            functionSpecifierList.push_back(FunctionSpecifier::INLINE);
            continue;
        }
        nextIndex = tagIndex1;
        return functionSpecifierList;
    }
}

std::vector<TypeQualifier> Parser::parseTypeQualifierList() {
    std::vector<TypeQualifier> typeQualifierList;
    while (true) {
        int tagIndex1 = nextIndex;
        Token token = getNextToken();
        if (token.id == TokenId::KEYWORD_CONST) {
            typeQualifierList.push_back(TypeQualifier::CONST);
            continue;
        }
        nextIndex = tagIndex1;
        return typeQualifierList;
    }
}

bool Parser::parseTypeSpecifier(BaseType &baseType) {
    int tagIndex1 = nextIndex;
    Token token = getNextToken();
    if (token.id == TokenId::KEYWORD_VOID) {
        baseType = BaseType::VOID;
        return true;
    } else if (token.id == TokenId::KEYWORD_CHAR) {
        baseType = BaseType::CHAR;
        return true;
    } else if (token.id == TokenId::KEYWORD_SHORT) {
        baseType = BaseType::SHORT;
        return true;
    } else if (token.id == TokenId::KEYWORD_INT) {
        baseType = BaseType::INT;
        return true;
    } else if (token.id == TokenId::KEYWORD_LONG) {
        int tagIndex2 = nextIndex;
        if (getNextToken().id == TokenId::KEYWORD_LONG) {
            int tagIndex3 = nextIndex;
            if (getNextToken().id != TokenId::KEYWORD_INT) {
                nextIndex = tagIndex3;
            }
            baseType = BaseType::LONG_LONG_INT;
            return true;
        }
        nextIndex = tagIndex2;
        baseType = BaseType::LONG_INT;
        return true;
    } else if (token.id == TokenId::KEYWORD_FLOAT) {
        baseType = BaseType::FLOAT;
        return true;
    } else if (token.id == TokenId::KEYWORD_DOUBLE) {
        baseType = BaseType::DOUBLE;
        return true;
    } else if (token.id == TokenId::KEYWORD_SIGNED) {
        int tagIndex2 = nextIndex;
        if (getNextToken().id == TokenId::KEYWORD_CHAR) {
            baseType = BaseType::CHAR;
            return true;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::KEYWORD_SHORT) {
            baseType = BaseType::SHORT;
            return true;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::KEYWORD_INT) {
            baseType = BaseType::INT;
            return true;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::KEYWORD_LONG) {
            int tagIndex3 = nextIndex;
            if (getNextToken().id == TokenId::KEYWORD_LONG) {
                int tagIndex4 = nextIndex;
                if (getNextToken().id != TokenId::KEYWORD_INT) {
                    nextIndex = tagIndex4;
                }
                baseType = BaseType::LONG_LONG_INT;
                return true;
            }
            nextIndex = tagIndex3;
            baseType = BaseType::LONG_INT;
            return true;
        }
        return false;
    } else if (token.id == TokenId::KEYWORD_UNSIGNED) {
        int tagIndex2 = nextIndex;
        if (getNextToken().id == TokenId::KEYWORD_CHAR) {
            baseType = BaseType::UNSIGNED_CHAR;
            return true;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::KEYWORD_SHORT) {
            baseType = BaseType::UNSIGNED_SHORT;
            return true;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::KEYWORD_INT) {
            baseType = BaseType::UNSIGNED_INT;
            return true;
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::KEYWORD_LONG) {
            int tagIndex3 = nextIndex;
            if (getNextToken().id == TokenId::KEYWORD_LONG) {
                int tagIndex4 = nextIndex;
                if (getNextToken().id != TokenId::KEYWORD_INT) {
                    nextIndex = tagIndex4;
                }
                baseType = BaseType::UNSIGNED_LONG_LONG_INT;
                return true;
            }
            nextIndex = tagIndex3;
            baseType = BaseType::UNSIGNED_LONG_INT;
            return true;
        }
        return false;
    }
    nextIndex = tagIndex1;
    return false;
}

std::vector<Expression *> Parser::parseInitializer() {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_CURLY_BRACES) {
        std::vector<Expression *> initialValueList = parseAssignmentExpressionList();
        if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES) {
            return initialValueList;
        }
        deleteAllElem(initialValueList);
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_CURLY_BRACES) {
        std::vector<Expression *> initialValueList = parseAssignmentExpressionList();
        if (getNextToken().id == TokenId::PUNCTUATOR_COMMA) {
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES) {
                return initialValueList;
            }
        }
        deleteAllElem(initialValueList);
    }
    nextIndex = tagIndex1;
    Expression *initialValue = parseAssignmentExpression();
    if (initialValue != nullptr) {
        return {initialValue};
    }
    delete initialValue;
    nextIndex = tagIndex1;
    return {};
}

bool Parser::parseDirectDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack) {
    int tagIndex1 = nextIndex;
    Token identifierToken = getNextToken();
    if (identifierToken.id == TokenId::IDENTIFIER) {
        identifierList.push_back(identifierToken.value);
        goto whileParseDirectDeclaratorSuffix;
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        if (parsePointerDeclarator(identifierList, typeStack)) {
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                goto whileParseDirectDeclaratorSuffix;
            }
        }
    }
    nextIndex = tagIndex1;
    return false;

    whileParseDirectDeclaratorSuffix:
    while (true) {
        int tagIndex2 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
            int tagIndex3 = nextIndex;
            Token sizeToken = getNextToken();
            if (sizeToken.id == TokenId::LITERAL_INTEGER) {
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                    typeStack.push(new ArrayType(nullptr, std::stoi(sizeToken.value)));
                    continue;
                }
            } else {
                nextIndex = tagIndex3;
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                    typeStack.push(new ArrayType(nullptr, 0));
                    continue;
                }
            }
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            std::vector<Type *> parameterTypeList = parseParameterList(identifierList);
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                typeStack.push(new FunctionType(nullptr, parameterTypeList));
                continue;
            }
            deleteAllElem(parameterTypeList);
        }
        nextIndex = tagIndex2;
        return true;
    }
}

bool Parser::parsePointerDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack) {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_MUL) {
        std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
        if (parsePointerDeclarator(identifierList, typeStack)) {
            typeStack.push(new PointerType(nullptr, typeQualifierList));
            return true;
        }
    }
    nextIndex = tagIndex1;
    if (parseDirectDeclarator(identifierList, typeStack)) {
        return true;
    }
    nextIndex = tagIndex1;
    return false;
}

bool Parser::parseDirectAbstractDeclarator(std::stack<Type *> &typeStack) {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        if (parsePointerAbstractDeclarator(typeStack)) {
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                goto whileParseDirectAbstractDeclaratorSuffix;
            }
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
        int tagIndex3 = nextIndex;
        if (getNextToken().id != TokenId::LITERAL_INTEGER) {
            nextIndex = tagIndex3;
        }
        if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
            typeStack.push(new ArrayType(nullptr, 0));
            goto whileParseDirectAbstractDeclaratorSuffix;
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
        std::vector<std::string> identifierList;
        std::vector<Type *> parameterTypeList = parseParameterList(identifierList);
        if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
            typeStack.push(new FunctionType(nullptr, parameterTypeList));
            goto whileParseDirectAbstractDeclaratorSuffix;
        }
        deleteAllElem(parameterTypeList);
    }
    nextIndex = tagIndex1;
    return false;

    whileParseDirectAbstractDeclaratorSuffix:
    while (true) {
        int tagIndex2 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_SQUARE_BRACKETS) {
            int tagIndex3 = nextIndex;
            if (getNextToken().id != TokenId::KEYWORD_INT) {
                nextIndex = tagIndex3;
            }
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_SQUARE_BRACKETS) {
                typeStack.push(new ArrayType(nullptr, 0));
                continue;
            }
        }
        nextIndex = tagIndex2;
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            std::vector<std::string> identifierList;
            std::vector<Type *> parameterTypeList = parseParameterList(identifierList);
            if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                typeStack.push(new FunctionType(nullptr, parameterTypeList));
                continue;
            }
            deleteAllElem(parameterTypeList);
        }
        nextIndex = tagIndex2;
        return true;
    }
}

bool Parser::parsePointerAbstractDeclarator(std::stack<Type *> &typeStack) {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_MUL) {
        std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
        parsePointerAbstractDeclarator(typeStack);
        typeStack.push(new PointerType(nullptr, typeQualifierList));
        return true;
    }
    nextIndex = tagIndex1;
    if (parseDirectAbstractDeclarator(typeStack)) {
        return true;
    }
    nextIndex = tagIndex1;
    return false;
}

Type *Parser::parseParameter(std::vector<std::string> &identifierList) {
    int tagIndex1 = nextIndex;
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        nextIndex = tagIndex1;
        return nullptr;
    }
    std::stack<Type *> typeStack;
    int tagIndex2 = nextIndex;
    if (parsePointerDeclarator(identifierList, typeStack)) {
        return typeStack2Type(typeQualifierList, typeSpecifier, typeStack);
    }
    deleteAndClearAllElem(typeStack);
    nextIndex = tagIndex2;
    if (parsePointerAbstractDeclarator(typeStack)) {
        identifierList.emplace_back(""); // 抽象声明不需要标识符，但是这里需要进行占位，以便后续定位到参数的标识符
        return typeStack2Type(typeQualifierList, typeSpecifier, typeStack);
    }
    nextIndex = tagIndex2;
    identifierList.emplace_back("");
    return new ScalarType(typeSpecifier, typeQualifierList);
}

std::vector<Type *> Parser::parseParameterList(std::vector<std::string> &identifierList) {
    std::vector<Type *> parameterTypeList;
    Type *parameterType = parseParameter(identifierList);
    if (parameterType == nullptr) {
        return {};
    }
    parameterTypeList.push_back(parameterType);
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_COMMA) {
            parameterType = parseParameter(identifierList);
            if (parameterType != nullptr) {
                parameterTypeList.push_back(parameterType);
                continue;
            }
        }
        nextIndex = tagIndex1;
        return parameterTypeList;
    }
}

bool Parser::parseInitDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack, std::vector<Expression *> &initialValueList) {
    int tagIndex1 = nextIndex;
    if (parsePointerDeclarator(identifierList, typeStack)) {
        if (getNextToken().id == TokenId::PUNCTUATOR_ASSIGN) {
            initialValueList = parseInitializer();
            if (!initialValueList.empty()) {
                return true;
            }
        }
    }
    clearAllElem(identifierList);
    deleteAndClearAllElem(typeStack);
    deleteAndClearAllElem(initialValueList);
    nextIndex = tagIndex1;
    if (parsePointerDeclarator(identifierList, typeStack)) {
        return true;
    }
    nextIndex = tagIndex1;
    return false;
}

bool Parser::parseInitDeclaratorList(std::vector<std::vector<std::string>> &identifierListList, std::vector<std::stack<Type *>> &typeStackList, std::vector<std::vector<Expression *>> &initialValueListList) {
    std::vector<std::string> identifierList;
    std::stack<Type *> typeStack;
    std::vector<Expression *> initialValueList;
    if (!parseInitDeclarator(identifierList, typeStack, initialValueList)) {
        deleteAndClearAllElem(typeStack);
        deleteAllElem(initialValueList);
        return false;
    }
    identifierListList.push_back(identifierList);
    typeStackList.push_back(typeStack);
    initialValueListList.push_back(initialValueList);
    while (true) {
        int tagIndex1 = nextIndex;
        if (getNextToken().id == TokenId::PUNCTUATOR_COMMA) {
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
            deleteAllElem(initialValueList);
        }
        nextIndex = tagIndex1;
        return true;
    }
}

std::vector<Declaration *> Parser::parseDeclaration() {
    int tagIndex1 = nextIndex;
    std::vector<FunctionSpecifier> functionSpecifierList = parseFunctionSpecifierList();
    std::vector<StorageSpecifier> storageSpecifierList = parseStorageSpecifierList();
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        nextIndex = tagIndex1;
        return {};
    }
    std::vector<std::vector<std::string>> identifierListList;
    std::vector<std::stack<Type *>> typeStackList;
    std::vector<std::vector<Expression *>> initialValueListList;
    if (parseInitDeclaratorList(identifierListList, typeStackList, initialValueListList)) {
        if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
            std::vector<Declaration *> declarationList;
            for (int i = 0; i < identifierListList.size(); i++) {
                Type *finalType = typeStack2Type(typeQualifierList, typeSpecifier, typeStackList[i]);
                switch (finalType->getClass()) {
                    case TypeClass::ARRAY_TYPE:
                        if (initialValueListList[i].size() == 1 && initialValueListList[i][0]->getClass() == ExpressionClass::STRING_LITERAL_EXPRESSION) {
                            std::string initialValue = ((StringLiteralExpression *) initialValueListList[i][0])->value;
                            initialValue.push_back('\0');
                            deleteAndClearAllElem(initialValueListList[i]);
                            for (char ch : initialValue) {
                                initialValueListList[i].push_back(new CharacterLiteralExpression(ch));
                            }
                        }
                        declarationList.push_back(new VariableDeclaration(storageSpecifierList, finalType, identifierListList[i][0], initialValueListList[i]));
                        break;
                    case TypeClass::FUNCTION_TYPE:
                        declarationList.push_back(new FunctionDeclaration(functionSpecifierList, finalType, identifierListList[i][0]));
                        deleteAllElem(initialValueListList[i]);
                        break;
                    case TypeClass::POINTER_TYPE:
                    case TypeClass::SCALAR_TYPE:
                        declarationList.push_back(new VariableDeclaration(storageSpecifierList, finalType, identifierListList[i][0], initialValueListList[i]));
                        break;
                }
            }
            return declarationList;
        }
    }
    for (auto typeStack : typeStackList) deleteAndClearAllElem(typeStack);
    for (auto initialValueList : initialValueListList) deleteAllElem(initialValueList);
    nextIndex = tagIndex1;
    return {};
}

Type *Parser::parseTypeName() {
    int tagIndex1 = nextIndex;
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        nextIndex = tagIndex1;
        return nullptr;
    }
    int tagIndex2 = nextIndex;
    std::stack<Type *> typeStack;
    if (parsePointerAbstractDeclarator(typeStack)) {
        return typeStack2Type(typeQualifierList, typeSpecifier, typeStack);
    }
    deleteAndClearAllElem(typeStack);
    nextIndex = tagIndex2;
    return new ScalarType(typeSpecifier, typeQualifierList);
}

Statement *Parser::parseBlockItem() {
    int tagIndex1 = nextIndex;
    std::vector<Declaration *> declarationList = parseDeclaration();
    if (!declarationList.empty()) {
        return new DeclarationStatement(declarationList);
    }
    deleteAllElem(declarationList);
    nextIndex = tagIndex1;
    Statement *blockItem = parseStatement();
    if (blockItem != nullptr) {
        return blockItem;
    }
    delete blockItem;
    nextIndex = tagIndex1;
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
    int tagIndex1 = nextIndex;
    Token identifierToken = getNextToken();
    if (identifierToken.id == TokenId::IDENTIFIER) {
        if (getNextToken().id == TokenId::PUNCTUATOR_COLON) {
            Statement *statement = parseStatement();
            if (statement != nullptr) {
                return new LabelStatement(identifierToken.value, statement);
            }
            delete statement;
        }
    }
    nextIndex = tagIndex1;
    if (identifierToken.id == TokenId::KEYWORD_CASE) {
        Token valueToken = getNextToken();
        if (valueToken.id == TokenId::KEYWORD_INT) {
            if (getNextToken().id == TokenId::PUNCTUATOR_COLON) {
                Statement *statement = parseStatement();
                if (statement != nullptr) {
                    return new CaseStatement(std::stoi(valueToken.value), statement);
                }
            }
        }
    }
    nextIndex = tagIndex1;
    if (identifierToken.id == TokenId::KEYWORD_DEFAULT) {
        if (getNextToken().id == TokenId::PUNCTUATOR_COLON) {
            Statement *statement = parseStatement();
            if (statement != nullptr) {
                return new DefaultStatement(statement);
            }
        }
    }
    nextIndex = tagIndex1;
    return nullptr;
}

Statement *Parser::parseCompoundStatement() {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_CURLY_BRACES) {
        std::vector<Statement *> statementList = parseBlockItemList();
        if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_CURLY_BRACES) {
            return new CompoundStatement(statementList);
        }
        deleteAllElem(statementList);
    }
    nextIndex = tagIndex1;
    return nullptr;
}

Statement *Parser::parseExpressionStatement() {
    int tagIndex1 = nextIndex;
    Expression *expression = parseCommaExpression();
    if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
        return new ExpressionStatement(expression);
    }
    delete expression;
    nextIndex = tagIndex1;
    return nullptr;
}

Statement *Parser::parseSelectionStatement() {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::KEYWORD_IF) {
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            Expression *condition = parseCommaExpression();
            if (condition != nullptr) {
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    Statement *trueBody = parseStatement();
                    if (trueBody != nullptr) {
                        if (getNextToken().id == TokenId::KEYWORD_ELSE) {
                            Statement *falseBody = parseStatement();
                            if (falseBody != nullptr) {
                                return new IfStatement(condition, trueBody, falseBody);
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
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_IF) {
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            Expression *condition = parseCommaExpression();
            if (condition != nullptr) {
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    Statement *trueBody = parseStatement();
                    if (trueBody != nullptr) {
                        return new IfStatement(condition, trueBody, nullptr);
                    }
                    delete trueBody;
                }
            }
            delete condition;
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_SWITCH) {
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            Expression *expression = parseCommaExpression();
            if (expression != nullptr) {
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    Statement *body = parseStatement();
                    if (body != nullptr) {
                        return new SwitchStatement(expression, body);
                    }
                    delete body;
                }
            }
            delete expression;
        }
    }
    nextIndex = tagIndex1;
    return nullptr;
}

Statement *Parser::parseIterationStatement() {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::KEYWORD_WHILE) {
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            Expression *condition = parseCommaExpression();
            if (condition != nullptr) {
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    Statement *body = parseStatement();
                    if (body != nullptr) {
                        return new WhileStatement(condition, body);
                    }
                    delete body;
                }
            }
            delete condition;
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_DO) {
        Statement *body = parseStatement();
        if (getNextToken().id == TokenId::KEYWORD_WHILE) {
            if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
                Expression *condition = parseCommaExpression();
                if (condition != nullptr) {
                    if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                        if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
                            return new DoWhileStatement(body, condition);
                        }
                    }
                }
                delete condition;
            }
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_FOR) {
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            Expression *initExpression = parseCommaExpression();
            if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
                Expression *condition = parseCommaExpression();
                if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
                    Expression *updateExpression = parseCommaExpression();
                    if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                        Statement *body = parseStatement();
                        if (body != nullptr) {
                            return new ForStatement({}, initExpression, condition, updateExpression, body);
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
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_FOR) {
        if (getNextToken().id == TokenId::PUNCTUATOR_LEFT_PARENTHESES) {
            std::vector<Declaration *> initDeclarationList = parseDeclaration();
            Expression *condition = parseCommaExpression();
            if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
                Expression *update = parseCommaExpression();
                if (getNextToken().id == TokenId::PUNCTUATOR_RIGHT_PARENTHESES) {
                    Statement *body = parseStatement();
                    if (body != nullptr) {
                        return new ForStatement(initDeclarationList, nullptr, condition, update, body);
                    }
                    delete body;
                }
                delete update;
            }
            delete condition;
            deleteAllElem(initDeclarationList);
        }
    }
    nextIndex = tagIndex1;
    return nullptr;
}

Statement *Parser::parseJumpStatement() {
    int tagIndex1 = nextIndex;
    if (getNextToken().id == TokenId::KEYWORD_GOTO) {
        Token identifierToken = getNextToken();
        if (identifierToken.id == TokenId::IDENTIFIER) {
            if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
                return new GotoStatement(identifierToken.value);
            }
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_CONTINUE) {
        if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
            return new ContinueStatement();
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_BREAK) {
        if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
            return new BreakStatement();
        }
    }
    nextIndex = tagIndex1;
    if (getNextToken().id == TokenId::KEYWORD_RETURN) {
        Expression *value = parseCommaExpression();
        if (getNextToken().id == TokenId::PUNCTUATOR_SEMICOLON) {
            return new ReturnStatement(value);
        }
        delete value;
    }
    nextIndex = tagIndex1;
    return nullptr;
}

Statement *Parser::parseStatement() {
    Statement *statement;
    int tagIndex1 = nextIndex;
    statement = parseLabeledStatement();
    if (statement != nullptr) {
        return statement;
    }
    nextIndex = tagIndex1;
    statement = parseCompoundStatement();
    if (statement != nullptr) {
        return statement;
    }
    nextIndex = tagIndex1;
    statement = parseExpressionStatement();
    if (statement != nullptr) {
        return statement;
    }
    nextIndex = tagIndex1;
    statement = parseSelectionStatement();
    if (statement != nullptr) {
        return statement;
    }
    nextIndex = tagIndex1;
    statement = parseIterationStatement();
    if (statement != nullptr) {
        return statement;
    }
    nextIndex = tagIndex1;
    statement = parseJumpStatement();
    if (statement != nullptr) {
        return statement;
    }
    nextIndex = tagIndex1;
    return nullptr;
}

Declaration *Parser::parseFunctionDefinition() {
    int tagIndex1 = nextIndex;
    std::vector<FunctionSpecifier> functionSpecifierList = parseFunctionSpecifierList();
    std::vector<StorageSpecifier> storageSpecifierList = parseStorageSpecifierList();
    std::vector<TypeQualifier> typeQualifierList = parseTypeQualifierList();
    BaseType typeSpecifier;
    if (!parseTypeSpecifier(typeSpecifier)) {
        nextIndex = tagIndex1;
        return {};
    }
    std::vector<std::string> identifierList;
    std::stack<Type *> typeStack;
    if (parsePointerDeclarator(identifierList, typeStack)) {
        Type *finalType = typeStack2Type(typeQualifierList, typeSpecifier, typeStack);
        if (finalType->getClass() == TypeClass::FUNCTION_TYPE) {
            std::vector<Type *> parameterTypeList = ((FunctionType *) finalType)->parameterTypeList;
            std::vector<std::string> parameterIdentifierList(identifierList.begin() + 1, identifierList.begin() + 1 + (int) (((FunctionType *) finalType)->parameterTypeList.size()));
            if (parameterTypeList.size() == parameterIdentifierList.size()) {
                if (!haveEmptyString(parameterIdentifierList)) {
                    Statement *body = parseCompoundStatement();
                    if (body != nullptr) {
                        std::vector<Declaration *> parameterDeclarationList;
                        for (int i = 0; i < parameterTypeList.size(); i++) {
                            if (parameterTypeList[i]->getClass() == TypeClass::FUNCTION_TYPE) {
                                parameterDeclarationList.push_back(new FunctionDeclaration({}, parameterTypeList[i]->clone(), parameterIdentifierList[i]));
                            } else {
                                parameterDeclarationList.push_back(new VariableDeclaration({}, parameterTypeList[i]->clone(), parameterIdentifierList[i], {}));
                            }
                        }
                        return new FunctionDefinition(functionSpecifierList, finalType, identifierList[0], parameterDeclarationList, body);
                    }
                    delete body;
                }
            }
        }
        delete finalType;
    }
    nextIndex = tagIndex1;
    return {};
}

std::vector<Declaration *> Parser::parseExternalDeclaration() {
    int tagIndex1 = nextIndex;
    Declaration *declaration = parseFunctionDefinition();
    if (declaration != nullptr) {
        return {declaration};
    }
    delete declaration;
    nextIndex = tagIndex1;
    std::vector<Declaration *> declarationList = parseDeclaration();
    if (!declarationList.empty()) {
        return declarationList;
    }
    deleteAllElem(declarationList);
    nextIndex = tagIndex1;
    return {};
}

TranslationUnit *Parser::parseTranslationUnit() {
    std::vector<Declaration *> declarationList;
    std::vector<Declaration *> newDeclarationList;
    while (!(newDeclarationList = parseExternalDeclaration()).empty()) {
        declarationList.insert(declarationList.end(), newDeclarationList.begin(), newDeclarationList.end());
    }
    return new TranslationUnit(declarationList);
}

TranslationUnit *Parser::analysis() {
    TranslationUnit *translationUnit = parseTranslationUnit();
    if (farthestIndex != tokenList->size() - 1) {
        haveError = true;
        std::string message = "syntax error of `";
        auto begin = farthestIndex < 5 ? tokenList->begin() : tokenList->begin() + farthestIndex - 5;
        auto end = farthestIndex + 5 > tokenList->size() - 1 ? tokenList->end() - 1 : tokenList->begin() + farthestIndex + 5;
        for (auto it = begin; it != end; it++) {
            message += (*it).value += " ";
        }
        if (message[message.size() - 1] == ' ') {
            message.erase(message.size() - 1);
        }
        message += "`";
        Logger::error(message);
    }
    return translationUnit;
}

bool Parser::isHaveError() const {
    return haveError;
}


