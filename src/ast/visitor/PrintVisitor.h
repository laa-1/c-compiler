#pragma once

#include "Visitor.h"
#include <string>

/**
 * 用于打印AST的visitor具体子类。
 * 使用时应当只调用TranslationUnit的visit函数。
 * 不应该复用同一个对象。
 */
class PrintVisitor : public Visitor {
private:
    std::string retract; // 某个节点的缩进前缀
    std::string label; // 某个节点的标签前缀

public:
    ~PrintVisitor() override = default;
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
};
