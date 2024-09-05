#pragma once

#include <memory>
#include <stack>
#include <queue>
#include "Visitor.h"
#include "../../constant/StringConstantPool.h"
#include "../../symbol/SymbolTable.h"
#include "../../symbol/SymbolTableIterator.h"
#include "../../instruction/InstructionSequenceBuilder.h"
#include "../../instruction/BinaryDataType.h"

/**
 * 用于对AST进行代码生成的visitor具体子类。
 * 使用时应当只调用TranslationUnit的visit函数。
 * 不应该复用同一个对象。
 */
class CodeGenerateVisitor : public Visitor {
private:
    SymbolTable *symbolTable = nullptr;
    StringConstantPool *stringConstantPool = nullptr;
    std::unique_ptr<SymbolTableIterator> symbolTableIterator = nullptr;
    std::unique_ptr<InstructionSequenceBuilder> instructionSequenceBuilder = std::make_unique<InstructionSequenceBuilder>();
    std::map<std::string, std::vector<int>> functionPlaceholderIndexMap; // 用来记录多个压入函数占位地址的push指令的索引，需要后续修改
    std::map<std::string, std::vector<int>> statementPlaceholderIndexMap; // 用来记录多个压入语句占位地址的push指令的索引，需要后续修改
    std::stack<std::vector<int>> switchPushIndexListStack; // 用来记录在switch语句中多个压入占位地址的push指令的索引，需要后续修改
    std::stack<std::vector<int>> breakPushIndexListStack; // 用于记录多个break语句中压入占位地址的push指令的索引，需要后续修改
    std::stack<std::vector<int>> continuePushIndexListStack; // 用于记录多个continue语句中压入占位地址的push指令的索引，需要后续修改（do-while循环使用）
    std::stack<std::uint64_t> continueJumpAddressStack; // 用于记录在continue的语句可以跳转的地址（while和for循环使用）
    bool needLoadValue = false;

private:
    void patchFunctionPlaceholderAddress(const std::string& identifier, std::uint64_t realAddress);
    void patchStatementPlaceholderAddress(const std::string& identifier, std::uint64_t realAddress);
    void patchBreakPushAddress(std::uint64_t realAddress);
    void patchContinuePushAddress(std::uint64_t realAddress);

public:
    CodeGenerateVisitor(SymbolTable *symbolTable, StringConstantPool *stringConstantPool);
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
    static InstructionSequence *generateCode(TranslationUnit *translationUnit, SymbolTable *symbolTable, StringConstantPool *stringConstantPool);
};
