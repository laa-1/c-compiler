#pragma once

#include <utility>
#include <vector>
#include <stack>
#include <memory>
#include "../lexer/Token.h"
#include "../ast/node/Expression.h"
#include "../ast/node/Type.h"
#include "../ast/node/Declaration.h"
#include "../ast/node/Statement.h"
#include "../ast/node/TranslationUnit.h"

class Parser {
private:
    std::vector<Token> *tokenList = nullptr;
    int nextIndex = 0; // 下一个token的索引
    int farthestIndex = 0; // 解析到的最远token的索引，用来检查是否全部解析完成，以及用于报告最有可能的错误位置
    bool haveError = false;

private:
    Token getNextToken();
    Expression *parsePrimaryExpression();
    Expression *parsePostfixExpression();
    Expression *parseUnaryExpression();
    Expression *parseCastExpression();
    Expression *parseMultiplicativeExpression();
    Expression *parseAdditiveExpression();
    Expression *parseShiftExpression();
    Expression *parseRelationalExpression();
    Expression *parseEqualityExpression();
    Expression *parseAndExpression();
    Expression *parseExclusiveOrExpression();
    Expression *parseInclusiveOrExpression();
    Expression *parseLogicalAndExpression();
    Expression *parseLogicalOrExpression();
    Expression *parseConditionalExpression();
    Expression *parseAssignmentExpression();
    Expression *parseCommaExpression();
    std::vector<Expression *> parseAssignmentExpressionList();
    std::vector<StorageSpecifier> parseStorageSpecifierList();
    std::vector<FunctionSpecifier> parseFunctionSpecifierList();
    std::vector<TypeQualifier> parseTypeQualifierList();
    bool parseTypeSpecifier(BaseType &baseType);
    std::vector<Expression *> parseInitializer();
    bool parseDirectDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack);
    bool parsePointerDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack);
    bool parseDirectAbstractDeclarator(std::stack<Type *> &typeStack);
    bool parsePointerAbstractDeclarator(std::stack<Type *> &typeStack);
    Type *parseParameter(std::vector<std::string> &identifierList);
    std::vector<Type *> parseParameterList(std::vector<std::string> &identifierList);
    bool parseInitDeclarator(std::vector<std::string> &identifierList, std::stack<Type *> &typeStack, std::vector<Expression *> &initialValueList);
    bool parseInitDeclaratorList(std::vector<std::vector<std::string>> &identifierListList, std::vector<std::stack<Type *>> &typeStackList, std::vector<std::vector<Expression *>> &initialValueListList);
    std::vector<Declaration *> parseDeclaration();
    Type *parseTypeName();
    Statement *parseBlockItem();
    std::vector<Statement *> parseBlockItemList();
    Statement *parseLabeledStatement();
    Statement *parseCompoundStatement();
    Statement *parseExpressionStatement();
    Statement *parseSelectionStatement();
    Statement *parseIterationStatement();
    Statement *parseJumpStatement();
    Statement *parseStatement();
    Declaration *parseFunctionDefinition();
    std::vector<Declaration *> parseExternalDeclaration();
    TranslationUnit *parseTranslationUnit();

public:
    explicit Parser(std::vector<Token> *tokenList);
    TranslationUnit *analysis();
    bool isHaveError() const;
};