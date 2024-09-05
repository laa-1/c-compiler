#pragma once

#include <stack>
#include <set>
#include "Visitor.h"
#include "../../constant/StringConstantPool.h"
#include "../../symbol/SymbolTableBuilder.h"

/**
 * 用于对AST进行错误检查的visitor具体子类。
 * 同时还会进行符号表的构建和字符串常量池的构建。
 * 使用时应当只调用TranslationUnit的visit函数。
 * 不应该复用同一个对象。
 */
class ErrorCheckVisitor : public Visitor {
private:
    std::unique_ptr<SymbolTableBuilder> symbolTableBuilder = std::make_unique<SymbolTableBuilder>();
    StringConstantPool *stringConstantPool = new StringConstantPool();
    bool haveEntryFunction = false;
    std::set<std::string> undefinedFunctionSet;
    FunctionType *currentFunctionType = nullptr;
    int levelSwitch = 0;
    int levelCanBreak = 0;
    int levelCanContinue = 0;

public:
    void visit(Declaration *declaration) override;
    void visit(Expression *expression) override;
    void visit(Statement *statement) override;
    void visit(Type *type) override;
    void visit(BinaryExpression *binaryExpression) override;
    void visit(CallExpression *callExpression) override;
    void visit(CastExpression *castExpression) override;
    void visit(CharacterLiteralExpression *characterLiteralExpression) override;
    void visit(FloatingPointLiteralExpression *floatingPointLiteralExpression) override;
    void visit(IdentifierExpression *identifierExpression) override;
    void visit(IntegerLiteralExpression *integerLiteralExpression) override;
    void visit(StringLiteralExpression *stringLiteralExpression) override;
    void visit(TernaryExpression *ternaryExpression) override;
    void visit(UnaryExpression *unaryExpression) override;
    void visit(ArrayType *arrayType) override;
    void visit(FunctionType *functionType) override;
    void visit(PointerType *pointerType) override;
    void visit(ScalarType *scalarType) override;
    void visit(FunctionDeclaration *functionDeclaration) override;
    void visit(FunctionDefinition *functionDefinition) override;
    void visit(VariableDeclaration *variableDeclaration) override;
    void visit(BreakStatement *breakStatement) override;
    void visit(CaseStatement *caseStatement) override;
    void visit(CompoundStatement *compoundStatement) override;
    void visit(ContinueStatement *continueStatement) override;
    void visit(DeclarationStatement *declarationStatement) override;
    void visit(DefaultStatement *defaultStatement) override;
    void visit(DoWhileStatement *doWhileStatement) override;
    void visit(ExpressionStatement *expressionStatement) override;
    void visit(ForStatement *forStatement) override;
    void visit(GotoStatement *gotoStatement) override;
    void visit(IfStatement *ifStatement) override;
    void visit(LabelStatement *labelStatement) override;
    void visit(ReturnStatement *returnStatement) override;
    void visit(SwitchStatement *switchStatement) override;
    void visit(WhileStatement *whileStatement) override;
    void visit(TranslationUnit *translationUnit) override;
    static void checkError(TranslationUnit *translationUnit, SymbolTable *&symbolTable, StringConstantPool *&stringConstantPool);
};
