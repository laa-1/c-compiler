#include "PrintVisitor.h"

#include <iostream>
#include <cassert>
#include "../node/Expression.h"
#include "../node/Type.h"
#include "../node/Declaration.h"
#include "../node/Statement.h"
#include "../node/TranslationUnit.h"
#include "../node/expression/BinaryExpression.h"
#include "../node/expression/CallExpression.h"
#include "../node/expression/CastExpression.h"
#include "../node/expression/CharacterLiteralExpression.h"
#include "../node/expression/FloatingPointLiteralExpression.h"
#include "../node/expression/IdentifierExpression.h"
#include "../node/expression/IntegerLiteralExpression.h"
#include "../node/expression/StringLiteralExpression.h"
#include "../node/expression/TernaryExpression.h"
#include "../node/expression/UnaryExpression.h"
#include "../node/type/ArrayType.h"
#include "../node/type/FunctionType.h"
#include "../node/type/PointerType.h"
#include "../node/type/ScalarType.h"
#include "../node/declaration/FunctionDeclaration.h"
#include "../node/declaration/FunctionDefinition.h"
#include "../node/declaration/VariableDeclaration.h"
#include "../node/statement/BreakStatement.h"
#include "../node/statement/CaseStatement.h"
#include "../node/statement/CompoundStatement.h"
#include "../node/statement/ContinueStatement.h"
#include "../node/statement/DeclarationStatement.h"
#include "../node/statement/DefaultStatement.h"
#include "../node/statement/DoWhileStatement.h"
#include "../node/statement/ExpressionStatement.h"
#include "../node/statement/ForStatement.h"
#include "../node/statement/GotoStatement.h"
#include "../node/statement/IfStatement.h"
#include "../node/statement/LabelStatement.h"
#include "../node/statement/ReturnStatement.h"
#include "../node/statement/SwitchStatement.h"
#include "../node/statement/WhileStatement.h"

std::string unaryOperator2String(const UnaryOperator &unaryOperator) {
    switch (unaryOperator) {
        case UnaryOperator::INCREMENT:
            return "++";
        case UnaryOperator::DECREMENT:
            return "--";
        case UnaryOperator::TAKE_ADDRESS:
            return "&";
        case UnaryOperator::DEREFERENCE:
            return "*";
        case UnaryOperator::PLUS:
            return "+";
        case UnaryOperator::MINUS:
            return "-";
        case UnaryOperator::BITWISE_NOT:
            return "~";
        case UnaryOperator::LOGICAL_NOT:
            return "!";
        case UnaryOperator::SIZEOF:
            return "sizeof";
    }
    assert(false);
}

std::string binaryOperator2String(const BinaryOperator &binaryOperator) {
    switch (binaryOperator) {
        case BinaryOperator::SUBSCRIPT:
            return "[]";
        case BinaryOperator::MUL:
            return "*";
        case BinaryOperator::DIV:
            return "/";
        case BinaryOperator::MOD:
            return "%";
        case BinaryOperator::ADD:
            return "+";
        case BinaryOperator::SUB:
            return "-";
        case BinaryOperator::SHIFT_LEFT:
            return "<<";
        case BinaryOperator::SHIFT_RIGHT:
            return ">>";
        case BinaryOperator::LESS:
            return "<";
        case BinaryOperator::GREATER:
            return ">";
        case BinaryOperator::LESS_EQUAL:
            return "<=";
        case BinaryOperator::GREATER_EQUAL:
            return ">=";
        case BinaryOperator::EQUAL:
            return "==";
        case BinaryOperator::NOT_EQUAL:
            return "!=";
        case BinaryOperator::BITWISE_AND:
            return "&";
        case BinaryOperator::BITWISE_XOR:
            return "^";
        case BinaryOperator::BITWISE_OR:
            return "|";
        case BinaryOperator::LOGICAL_AND:
            return "&&";
        case BinaryOperator::LOGICAL_OR:
            return "||";
        case BinaryOperator::ASSIGN:
            return "=";
        case BinaryOperator::MUL_ASSIGN:
            return "*=";
        case BinaryOperator::DIV_ASSIGN:
            return "/=";
        case BinaryOperator::MOD_ASSIGN:
            return "%=";
        case BinaryOperator::ADD_ASSIGN:
            return "+=";
        case BinaryOperator::SUB_ASSIGN:
            return "-=";
        case BinaryOperator::SHIFT_LEFT_ASSIGN:
            return "<<=";
        case BinaryOperator::SHIFT_RIGHT_ASSIGN:
            return ">>=";
        case BinaryOperator::BITWISE_AND_ASSIGN:
            return "&=";
        case BinaryOperator::BITWISE_XOR_ASSIGN:
            return "^=";
        case BinaryOperator::BITWISE_OR_ASSIGN:
            return "|=";
        case BinaryOperator::COMMA:
            return ",";
    }
    assert(false);
}

std::string ternaryOperator2String(const TernaryOperator &ternaryOperator) {
    switch (ternaryOperator) {
        case TernaryOperator::CONDITION:
            return "?:";
    }
    assert(false);
}

std::string typeQualifier2String(const TypeQualifier &typeQualifier) {
    switch (typeQualifier) {
        case TypeQualifier::CONST:
            return "const";
    }
    assert(false);
}

std::string baseType2String(const BaseType &typeSpecifier) {
    switch (typeSpecifier) {
        case BaseType::VOID:
            return "void";
        case BaseType::CHAR:
            return "char";
        case BaseType::SHORT:
            return "short";
        case BaseType::INT:
            return "int";
        case BaseType::LONG_INT:
            return "long int";
        case BaseType::LONG_LONG_INT:
            return "long long int";
        case BaseType::UNSIGNED_CHAR:
            return "unsigned char";
        case BaseType::UNSIGNED_SHORT:
            return "unsigned short";
        case BaseType::UNSIGNED_INT:
            return "unsigned int";
        case BaseType::UNSIGNED_LONG_INT:
            return "unsigned long int";
        case BaseType::UNSIGNED_LONG_LONG_INT:
            return "unsigned long long int";
        case BaseType::FLOAT:
            return "float";
        case BaseType::DOUBLE:
            return "double";
    }
    assert(false);
}

std::string storageSpecifier2String(const StorageSpecifier &storageSpecifier) {
    switch (storageSpecifier) {
        case StorageSpecifier::EXTERN:
            return "extern";
        case StorageSpecifier::STATIC:
            return "static";
    }
    assert(false);
}

std::string functionSpecifier2String(const FunctionSpecifier &functionSpecifier) {
    switch (functionSpecifier) {
        case FunctionSpecifier::INLINE:
            return "inline";
    }
    assert(false);
}

void PrintVisitor::visit(Declaration *declaration) {
    switch (declaration->getClass()) {
        case DeclarationClass::FUNCTION_DECLARATION:
            visit((FunctionDeclaration *) declaration);
            break;
        case DeclarationClass::FUNCTION_DEFINITION:
            visit((FunctionDefinition *) declaration);
            break;
        case DeclarationClass::VARIABLE_DECLARATION:
            visit((VariableDeclaration *) declaration);
            break;
    }
}

void PrintVisitor::visit(Expression *expression) {
    switch (expression->getClass()) {
        case ExpressionClass::BINARY_EXPRESSION:
            visit((BinaryExpression *) expression);
            break;
        case ExpressionClass::CALL_EXPRESSION:
            visit((CallExpression *) expression);
            break;
        case ExpressionClass::CAST_EXPRESSION:
            visit((CastExpression *) expression);
            break;
        case ExpressionClass::CHAR_LITERAL_EXPRESSION:
            visit((CharacterLiteralExpression *) expression);
            break;
        case ExpressionClass::FLOAT_LITERAL_EXPRESSION:
            visit((FloatingPointLiteralExpression *) expression);
            break;
        case ExpressionClass::IDENTIFIER_EXPRESSION:
            visit((IdentifierExpression *) expression);
            break;
        case ExpressionClass::INT_LITERAL_EXPRESSION:
            visit((IntegerLiteralExpression *) expression);
            break;
        case ExpressionClass::STRING_LITERAL_EXPRESSION:
            visit((StringLiteralExpression *) expression);
            break;
        case ExpressionClass::TERNARY_EXPRESSION:
            visit((TernaryExpression *) expression);
            break;
        case ExpressionClass::UNARY_EXPRESSION:
            visit((UnaryExpression *) expression);
            break;
    }
}

void PrintVisitor::visit(Statement *statement) {
    switch (statement->getClass()) {
        case StatementClass::BREAK_STATEMENT:
            visit((BreakStatement *) statement);
            break;
        case StatementClass::CASE_STATEMENT:
            visit((CaseStatement *) statement);
            break;
        case StatementClass::COMPOUND_STATEMENT:
            visit((CompoundStatement *) statement);
            break;
        case StatementClass::CONTINUE_STATEMENT:
            visit((ContinueStatement *) statement);
            break;
        case StatementClass::DECLARATION_STATEMENT:
            visit((DeclarationStatement *) statement);
            break;
        case StatementClass::DEFAULT_STATEMENT:
            visit((DefaultStatement *) statement);
            break;
        case StatementClass::DO_WHILE_STATEMENT:
            visit((DoWhileStatement *) statement);
            break;
        case StatementClass::EXPRESSION_STATEMENT:
            visit((ExpressionStatement *) statement);
            break;
        case StatementClass::FOR_STATEMENT:
            visit((ForStatement *) statement);
            break;
        case StatementClass::GOTO_STATEMENT:
            visit((GotoStatement *) statement);
            break;
        case StatementClass::IF_STATEMENT:
            visit((IfStatement *) statement);
            break;
        case StatementClass::LABEL_STATEMENT:
            visit((LabelStatement *) statement);
            break;
        case StatementClass::RETURN_STATEMENT:
            visit((ReturnStatement *) statement);
            break;
        case StatementClass::SWITCH_STATEMENT:
            visit((SwitchStatement *) statement);
            break;
        case StatementClass::WHILE_STATEMENT:
            visit((WhileStatement *) statement);
            break;
    }
}

void PrintVisitor::visit(Type *type) {
    switch (type->getClass()) {
        case TypeClass::ARRAY_TYPE:
            visit((ArrayType *) type);
            break;
        case TypeClass::FUNCTION_TYPE:
            visit((FunctionType *) type);
            break;
        case TypeClass::POINTER_TYPE:
            visit((PointerType *) type);
            break;
        case TypeClass::SCALAR_TYPE:
            visit((ScalarType *) type);
            break;
    }
}

void PrintVisitor::visit(BinaryExpression *binaryExpression) {
    std::cout << retract << label << "BinaryExpression" << std::endl;
    retract += "    ";
    std::cout << retract << "binaryOperator: " << binaryOperator2String(binaryExpression->binaryOperator) << std::endl;
    label = "leftOperand: ";
    visit(binaryExpression->leftOperand);
    label = "rightOperand: ";
    visit(binaryExpression->rightOperand);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(CallExpression *callExpression) {
    std::cout << retract << label << "CallExpression" << std::endl;
    retract += "    ";
    label = "functionAddress: ";
    visit(callExpression->functionAddress);
    for (auto argument : callExpression->argumentList) {
        label = "argument: ";
        visit(argument);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(CastExpression *castExpression) {
    std::cout << retract << label << "CastExpression" << std::endl;
    retract += "    ";
    label = "targetType: ";
    visit(castExpression->targetType);
    label = "operand: ";
    visit(castExpression->operand);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(CharacterLiteralExpression *characterLiteralExpression) {
    std::cout << retract << label << "CharLiteralExpression" << std::endl;
    retract += "    ";
    std::cout << retract << "value: " << characterLiteralExpression->value << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(FloatingPointLiteralExpression *floatingPointLiteralExpression) {
    std::cout << retract << label << "DoubleLiteralExpression" << std::endl;
    retract += "    ";
    std::cout << retract << "value: " << floatingPointLiteralExpression->value << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(IdentifierExpression *identifierExpression) {
    std::cout << retract << label << "IdentifierExpression" << std::endl;
    retract += "    ";
    std::cout << retract << "identifier: " << identifierExpression->identifier << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(IntegerLiteralExpression *integerLiteralExpression) {
    std::cout << retract << label << "IntLiteralExpression" << std::endl;
    retract += "    ";
    std::cout << retract << "value: " << integerLiteralExpression->value << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(StringLiteralExpression *stringLiteralExpression) {
    std::cout << retract << label << "StringLiteralExpression" << std::endl;
    retract += "    ";
    std::cout << retract << "value: " << stringLiteralExpression->value << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(TernaryExpression *ternaryExpression) {
    std::cout << retract << label << "TernaryExpression" << std::endl;
    retract += "    ";
    std::cout << retract << "ternaryOperator: " << ternaryOperator2String(ternaryExpression->ternaryOperator) << std::endl;
    label = "leftOperand: ";
    visit(ternaryExpression->leftOperand);
    label = "middleOperand: ";
    visit(ternaryExpression->middleOperand);
    label = "rightOperand: ";
    visit(ternaryExpression->rightOperand);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(UnaryExpression *unaryExpression) {
    std::cout << retract << label << "UnaryExpression" << std::endl;
    retract += "    ";
    std::cout << retract << label << "unaryOperator: " << unaryOperator2String(unaryExpression->unaryOperator) << std::endl;
    label = "operand: ";
    visit(unaryExpression->operand);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(ArrayType *arrayType) {
    std::cout << retract << label << "ArrayType" << std::endl;
    retract += "    ";
    label = "elemType: ";
    visit(arrayType->elemType);
    std::cout << retract << "size: " << arrayType->size << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(FunctionType *functionType) {
    std::cout << retract << label << "FunctionType" << std::endl;
    retract += "    ";
    label = "returnType: ";
    visit(functionType->returnType);
    for (auto parameterType : functionType->parameterTypeList) {
        label = "parameterType: ";
        visit(parameterType);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(PointerType *pointerType) {
    std::cout << retract << label << "PointerType" << std::endl;
    retract += "    ";
    label = "sourceType: ";
    visit(pointerType->sourceType);
    for (auto typeQualifier : pointerType->typeQualifierList) {
        std::cout << retract << "typeQualifier: " << typeQualifier2String(typeQualifier) << std::endl;
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(ScalarType *scalarType) {
    std::cout << retract << label << "ScalarType" << std::endl;
    retract += "    ";
    std::cout << retract << "baseType: " << baseType2String(scalarType->baseType) << std::endl;
    for (auto typeQualifier : scalarType->typeQualifierList) {
        std::cout << retract << "typeQualifier: " << typeQualifier2String(typeQualifier) << std::endl;
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(FunctionDeclaration *functionDeclaration) {
    std::cout << retract << label << "FunctionDeclaration" << std::endl;
    retract += "    ";
    for (auto functionSpecifier : functionDeclaration->functionSpecifierList) {
        std::cout << retract << "functionSpecifier: " << functionSpecifier2String(functionSpecifier) << std::endl;
    }
    label = "functionType: ";
    visit(functionDeclaration->functionType);
    std::cout << retract << "identifier: " << functionDeclaration->identifier << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(FunctionDefinition *functionDefinition) {
    std::cout << retract << label << "FunctionDefinition" << std::endl;
    retract += "    ";
    for (auto functionSpecifier : functionDefinition->functionSpecifierList) {
        std::cout << retract << "functionSpecifier: " << functionSpecifier2String(functionSpecifier) << std::endl;
    }
    label = "functionType: ";
    visit(functionDefinition->functionType);
    std::cout << retract << "identifier: " << functionDefinition->identifier << std::endl;
    for (const auto &parameterDeclaration : functionDefinition->parameterDeclarationList) {
        label = "parameterDeclaration: ";
        visit(parameterDeclaration);
    }
    label = "body: ";
    visit(functionDefinition->body);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(VariableDeclaration *variableDeclaration) {
    std::cout << retract << label << "VariableDeclaration" << std::endl;
    retract += "    ";
    for (auto storageSpecifier : variableDeclaration->storageSpecifierList) {
        std::cout << retract << "storageSpecifier: " << storageSpecifier2String(storageSpecifier) << std::endl;
    }
    label = "variableType: ";
    visit(variableDeclaration->variableType);
    std::cout << retract << "identifier: " << variableDeclaration->identifier << std::endl;
    for (auto initialValue : variableDeclaration->initialValueList) {
        label = "initialValue: ";
        visit(initialValue);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(BreakStatement *breakStatement) {
    std::cout << retract << label << "BreakStatement" << std::endl;
}

void PrintVisitor::visit(CaseStatement *caseStatement) {
    std::cout << retract << label << "CaseStatement" << std::endl;
    retract += "    ";
    std::cout << retract << "value: " << caseStatement->value << std::endl;
    label = "statement: ";
    visit(caseStatement->statement);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(CompoundStatement *compoundStatement) {
    std::cout << retract << label << "CompoundStatement" << std::endl;
    retract += "    ";
    for (auto statement : compoundStatement->statementList) {
        label = "statement: ";
        visit(statement);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(ContinueStatement *continueStatement) {
    std::cout << retract << label << "ContinueStatement" << std::endl;
}

void PrintVisitor::visit(DeclarationStatement *declarationStatement) {
    std::cout << retract << label << "DeclarationStatement" << std::endl;
    retract += "    ";
    for (auto declaration : declarationStatement->declarationList) {
        label = "declaration: ";
        visit(declaration);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(DefaultStatement *defaultStatement) {
    std::cout << retract << label << "DefaultStatement" << std::endl;
    retract += "    ";
    label = "statement: ";
    visit(defaultStatement->statement);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(DoWhileStatement *doWhileStatement) {
    std::cout << retract << label << "DoWhileStatement" << std::endl;
    retract += "    ";
    label = "body: ";
    visit(doWhileStatement->body);
    label = "condition: ";
    visit(doWhileStatement->condition);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(ExpressionStatement *expressionStatement) {
    std::cout << retract << label << "ExpressionStatement" << std::endl;
    retract += "    ";
    if (expressionStatement->expression != nullptr) {
        label = "expression: ";
        visit(expressionStatement->expression);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(ForStatement *forStatement) {
    std::cout << retract << label << "ForStatement" << std::endl;
    retract += "    ";
    for (auto initDeclaration : forStatement->declarationList) {
        label = "declaration: ";
        visit(initDeclaration);
    }
    if (forStatement->init != nullptr) {
        label = "init: ";
        visit(forStatement->init);
    }
    if (forStatement->condition != nullptr) {
        label = "condition: ";
        visit(forStatement->condition);
    }
    if (forStatement->update != nullptr) {
        label = "update: ";
        visit(forStatement->update);
    }
    label = "body: ";
    visit(forStatement->body);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(GotoStatement *gotoStatement) {
    std::cout << retract << label << "GotoStatement" << std::endl;
    retract += "    ";
    std::cout << retract << "identifier: " << gotoStatement->identifier << std::endl;
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(IfStatement *ifStatement) {
    std::cout << retract << label << "IfStatement" << std::endl;
    retract += "    ";
    label = "condition: ";
    visit(ifStatement->condition);
    label = "trueBody: ";
    visit(ifStatement->trueBody);
    if (ifStatement->falseBody != nullptr) {
        label = "falseBody: ";
        visit(ifStatement->falseBody);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(LabelStatement *labelStatement) {
    std::cout << retract << label << "LabelStatement" << std::endl;
    retract += "    ";
    std::cout << retract << "identifier: " << labelStatement->identifier << std::endl;
    label = "statement: ";
    visit(labelStatement->statement);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(ReturnStatement *returnStatement) {
    std::cout << retract << label << "ReturnStatement" << std::endl;
    retract += "    ";
    if (returnStatement->value != nullptr) {
        label = "value: ";
        visit(returnStatement->value);
    }
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(SwitchStatement *switchStatement) {
    std::cout << retract << label << "SwitchStatement" << std::endl;
    retract += "    ";
    label = "expression: ";
    visit(switchStatement->expression);
    label = "body: ";
    visit(switchStatement->body);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(WhileStatement *whileStatement) {
    std::cout << retract << label << "WhileStatement" << std::endl;
    retract += "    ";
    label = "condition: ";
    visit(whileStatement->condition);
    label = "body: ";
    visit(whileStatement->body);
    retract.erase(retract.size() - 4);
}

void PrintVisitor::visit(TranslationUnit *translationUnit) {
    std::cout << retract << label << "TranslationUnit" << std::endl;
    retract += "    ";
    for (auto declaration : translationUnit->declarationList) {
        label = "declaration: ";
        visit(declaration);
    }
    retract.erase(retract.size() - 4);
}

