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
    TranslationUnit *translationUnit = nullptr;
    Token token;
    int currentIndex = 0;
    int farthestIndex = 0;

private:
    explicit Parser(std::vector<Token> *tokenList);
    inline void nextToken();
    inline void rollbackToken(int index);
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
    void analysis();

public:
    static TranslationUnit *analysis(std::vector<Token> *tokenList);
};