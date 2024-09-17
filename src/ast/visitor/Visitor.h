#pragma once

#include <memory>

class Expression;

class Type;

class Declaration;

class Statement;

class BinaryExpression;

class CallExpression;

class CastExpression;

class CharacterLiteralExpression;

class FloatingPointLiteralExpression;

class IdentifierExpression;

class IntegerLiteralExpression;

class StringLiteralExpression;

class TernaryExpression;

class UnaryExpression;

class ArrayType;

class ScalarType;

class FunctionType;

class PointerType;

class FunctionDeclaration;

class FunctionDefinition;

class VariableDeclaration;

class BreakStatement;

class CaseStatement;

class CompoundStatement;

class ContinueStatement;

class DeclarationStatement;

class DefaultStatement;

class DoWhileStatement;

class ExpressionStatement;

class ForStatement;

class GotoStatement;

class IfStatement;

class LabelStatement;

class ReturnStatement;

class SwitchStatement;

class WhileStatement;

class TranslationUnit;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Declaration *declaration) = 0;
    virtual void visit(Expression *expression) = 0;
    virtual void visit(Statement *statement) = 0;
    virtual void visit(Type *type) = 0;
    virtual void visit(BinaryExpression *binaryExpression) = 0;
    virtual void visit(CallExpression *callExpression) = 0;
    virtual void visit(CastExpression *castExpression) = 0;
    virtual void visit(CharacterLiteralExpression *characterLiteralExpression) = 0;
    virtual void visit(FloatingPointLiteralExpression *floatingPointLiteralExpression) = 0;
    virtual void visit(IdentifierExpression *identifierExpression) = 0;
    virtual void visit(IntegerLiteralExpression *integerLiteralExpression) = 0;
    virtual void visit(StringLiteralExpression *stringLiteralExpression) = 0;
    virtual void visit(TernaryExpression *ternaryExpression) = 0;
    virtual void visit(UnaryExpression *unaryExpression) = 0;
    virtual void visit(ArrayType *arrayType) = 0;
    virtual void visit(ScalarType *scalarType) = 0;
    virtual void visit(FunctionType *functionType) = 0;
    virtual void visit(PointerType *pointerType) = 0;
    virtual void visit(FunctionDeclaration *functionDeclaration) = 0;
    virtual void visit(FunctionDefinition *functionDefinition) = 0;
    virtual void visit(VariableDeclaration *variableDeclaration) = 0;
    virtual void visit(BreakStatement *breakStatement) = 0;
    virtual void visit(CaseStatement *caseStatement) = 0;
    virtual void visit(CompoundStatement *compoundStatement) = 0;
    virtual void visit(ContinueStatement *continueStatement) = 0;
    virtual void visit(DeclarationStatement *declarationStatement) = 0;
    virtual void visit(DefaultStatement *defaultStatement) = 0;
    virtual void visit(DoWhileStatement *doWhileStatement) = 0;
    virtual void visit(ExpressionStatement *expressionStatement) = 0;
    virtual void visit(ForStatement *forStatement) = 0;
    virtual void visit(GotoStatement *gotoStatement) = 0;
    virtual void visit(IfStatement *ifStatement) = 0;
    virtual void visit(LabelStatement *labelStatement) = 0;
    virtual void visit(ReturnStatement *returnStatement) = 0;
    virtual void visit(SwitchStatement *switchStatement) = 0;
    virtual void visit(WhileStatement *whileStatement) = 0;
    virtual void visit(TranslationUnit *translationUnit) = 0;
};
