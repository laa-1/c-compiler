#include "CodeGenerateVisitor.h"

#include <cassert>
#include <map>
#include "../node/Declaration.h"
#include "../node/Statement.h"
#include "../node/TranslationUnit.h"
#include "../node/Type.h"
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
#include "../../symbol/ArraySymbol.h"
#include "../../symbol/FunctionSymbol.h"
#include "../../symbol/PointerSymbol.h"
#include "../../symbol/ScalarSymbol.h"
#include "../../symbol/StatementSymbol.h"
#include "../../builtin/BuiltInFunctionInserter.h"

int getTypeByteNum(Type *type) {
    switch (type->getClass()) {
        case TypeClass::SCALAR_TYPE:
            switch (reinterpret_cast<ScalarType *>(type)->baseType) {
                case BaseType::VOID:
                    assert(false);
                case BaseType::CHAR:
                case BaseType::UNSIGNED_CHAR:
                    return 1;
                case BaseType::SHORT:
                case BaseType::UNSIGNED_SHORT:
                    return 2;
                case BaseType::INT:
                case BaseType::LONG_INT:
                case BaseType::UNSIGNED_INT:
                case BaseType::UNSIGNED_LONG_INT:
                case BaseType::FLOAT:
                    return 4;
                case BaseType::LONG_LONG_INT:
                case BaseType::UNSIGNED_LONG_LONG_INT:
                case BaseType::DOUBLE:
                    return 8;
            }
        case TypeClass::ARRAY_TYPE:
        case TypeClass::FUNCTION_TYPE:
        case TypeClass::POINTER_TYPE:
            return 8;
    }
    assert(false);
}

BinaryDataType type2BinaryDataType(Type *type) {
    switch (type->getClass()) {
        case TypeClass::SCALAR_TYPE:
            switch (reinterpret_cast<ScalarType *>(type)->baseType) {
                case BaseType::VOID:
                    assert(false);
                case BaseType::CHAR:
                    return BinaryDataType::I8;
                case BaseType::SHORT:
                    return BinaryDataType::I16;
                case BaseType::INT:
                case BaseType::LONG_INT:
                    return BinaryDataType::I32;
                case BaseType::LONG_LONG_INT:
                    return BinaryDataType::I64;
                case BaseType::UNSIGNED_CHAR:
                    return BinaryDataType::U8;
                case BaseType::UNSIGNED_SHORT:
                    return BinaryDataType::U16;
                case BaseType::UNSIGNED_INT:
                case BaseType::UNSIGNED_LONG_INT:
                    return BinaryDataType::U32;
                case BaseType::UNSIGNED_LONG_LONG_INT:
                    return BinaryDataType::U64;
                case BaseType::FLOAT:
                    return BinaryDataType::F32;
                case BaseType::DOUBLE:
                    return BinaryDataType::F64;
            }
        case TypeClass::ARRAY_TYPE:
        case TypeClass::FUNCTION_TYPE:
        case TypeClass::POINTER_TYPE:
            return BinaryDataType::U64;
    }
    assert(false);
}

BinaryDataType getLagerBinaryDataType(BinaryDataType binaryDataType1, BinaryDataType binaryDataType2) {
    if (binaryDataType1 == BinaryDataType::F64 || binaryDataType2 == BinaryDataType::F64) {
        return BinaryDataType::F64;
    } else if (binaryDataType1 == BinaryDataType::U64 || binaryDataType2 == BinaryDataType::U64) {
        return BinaryDataType::U64;
    } else if (binaryDataType1 == BinaryDataType::I64 || binaryDataType2 == BinaryDataType::I64) {
        return BinaryDataType::I64;
    } else if (binaryDataType1 == BinaryDataType::U32 || binaryDataType2 == BinaryDataType::U32) {
        return BinaryDataType::U32;
    } else if (binaryDataType1 == BinaryDataType::I32 || binaryDataType2 == BinaryDataType::I32) {
        return BinaryDataType::I32;
    } else if (binaryDataType1 == BinaryDataType::U16 || binaryDataType2 == BinaryDataType::U16) {
        return BinaryDataType::U16;
    } else if (binaryDataType1 == BinaryDataType::I16 || binaryDataType2 == BinaryDataType::I16) {
        return BinaryDataType::I16;
    } else if (binaryDataType1 == BinaryDataType::U8 || binaryDataType2 == BinaryDataType::U8) {
        return BinaryDataType::U8;
    } else if (binaryDataType1 == BinaryDataType::I8 || binaryDataType2 == BinaryDataType::I8) {
        return BinaryDataType::I8;
    }
    assert(false);
}

CodeGenerateVisitor::CodeGenerateVisitor(SymbolTable *symbolTable, StringConstantPool *stringConstantPool) : symbolTable(symbolTable), stringConstantPool(stringConstantPool) {
    this->symbolTableIterator = std::unique_ptr<SymbolTableIterator>(this->symbolTable->createIterator());
}

void CodeGenerateVisitor::patchFunctionPlaceholderAddress(const std::string& identifier, std::uint64_t realAddress) {
    if (functionPlaceholderIndexMap.contains(identifier)) {
        for (auto instructionIndex : functionPlaceholderIndexMap[identifier]) {
            instructionSequenceBuilder->modifyPush(instructionIndex, realAddress);
        }
        functionPlaceholderIndexMap.erase(identifier);
    }
}

void CodeGenerateVisitor::patchStatementPlaceholderAddress(const std::string& identifier, std::uint64_t realAddress) {
    if (statementPlaceholderIndexMap.contains(identifier)) {
        for (auto instructionIndex : statementPlaceholderIndexMap[identifier]) {
            instructionSequenceBuilder->modifyPush(instructionIndex, realAddress);
        }
        statementPlaceholderIndexMap.erase(identifier);
    }
}

void CodeGenerateVisitor::patchBreakPushAddress(std::uint64_t realAddress) {
    for (auto instructionIndex : breakPushIndexListStack.top()) {
        instructionSequenceBuilder->modifyPush(instructionIndex, realAddress);
    }
}

void CodeGenerateVisitor::patchContinuePushAddress(std::uint64_t realAddress) {
    for (auto instructionIndex : continuePushIndexListStack.top()) {
        instructionSequenceBuilder->modifyPush(instructionIndex, realAddress);
    }
}

void CodeGenerateVisitor::visit(Declaration *declaration) {
    switch (declaration->getClass()) {
        case DeclarationClass::FUNCTION_DECLARATION:
            visit(reinterpret_cast<FunctionDeclaration *>(declaration));
            break;
        case DeclarationClass::FUNCTION_DEFINITION:
            visit(reinterpret_cast<FunctionDefinition *>(declaration));
            break;
        case DeclarationClass::VARIABLE_DECLARATION:
            visit(reinterpret_cast<VariableDeclaration *>(declaration));
            break;
    }
}

void CodeGenerateVisitor::visit(Expression *expression) {
    switch (expression->getClass()) {
        case ExpressionClass::BINARY_EXPRESSION:
            visit(reinterpret_cast<BinaryExpression *>(expression));
            break;
        case ExpressionClass::CALL_EXPRESSION:
            visit(reinterpret_cast<CallExpression *>(expression));
            break;
        case ExpressionClass::CAST_EXPRESSION:
            visit(reinterpret_cast<CastExpression *>(expression));
            break;
        case ExpressionClass::CHAR_LITERAL_EXPRESSION:
            visit(reinterpret_cast<CharacterLiteralExpression *>(expression));
            break;
        case ExpressionClass::FLOAT_LITERAL_EXPRESSION:
            visit(reinterpret_cast<FloatingPointLiteralExpression *>(expression));
            break;
        case ExpressionClass::IDENTIFIER_EXPRESSION:
            visit(reinterpret_cast<IdentifierExpression *>(expression));
            break;
        case ExpressionClass::INT_LITERAL_EXPRESSION:
            visit(reinterpret_cast<IntegerLiteralExpression *>(expression));
            break;
        case ExpressionClass::STRING_LITERAL_EXPRESSION:
            visit(reinterpret_cast<StringLiteralExpression *>(expression));
            break;
        case ExpressionClass::TERNARY_EXPRESSION:
            visit(reinterpret_cast<TernaryExpression *>(expression));
            break;
        case ExpressionClass::UNARY_EXPRESSION:
            visit(reinterpret_cast<UnaryExpression *>(expression));
            break;
    }
}

void CodeGenerateVisitor::visit(Statement *statement) {
    switch (statement->getClass()) {
        case StatementClass::BREAK_STATEMENT:
            visit(reinterpret_cast<BreakStatement *>(statement));
            break;
        case StatementClass::CASE_STATEMENT:
            visit(reinterpret_cast<CaseStatement *>(statement));
            break;
        case StatementClass::COMPOUND_STATEMENT:
            visit(reinterpret_cast<CompoundStatement *>(statement));
            break;
        case StatementClass::CONTINUE_STATEMENT:
            visit(reinterpret_cast<ContinueStatement *>(statement));
            break;
        case StatementClass::DECLARATION_STATEMENT:
            visit(reinterpret_cast<DeclarationStatement *>(statement));
            break;
        case StatementClass::DEFAULT_STATEMENT:
            visit(reinterpret_cast<DefaultStatement *>(statement));
            break;
        case StatementClass::DO_WHILE_STATEMENT:
            visit(reinterpret_cast<DoWhileStatement *>(statement));
            break;
        case StatementClass::EXPRESSION_STATEMENT:
            visit(reinterpret_cast<ExpressionStatement *>(statement));
            break;
        case StatementClass::FOR_STATEMENT:
            visit(reinterpret_cast<ForStatement *>(statement));
            break;
        case StatementClass::GOTO_STATEMENT:
            visit(reinterpret_cast<GotoStatement *>(statement));
            break;
        case StatementClass::IF_STATEMENT:
            visit(reinterpret_cast<IfStatement *>(statement));
            break;
        case StatementClass::LABEL_STATEMENT:
            visit(reinterpret_cast<LabelStatement *>(statement));
            break;
        case StatementClass::RETURN_STATEMENT:
            visit(reinterpret_cast<ReturnStatement *>(statement));
            break;
        case StatementClass::SWITCH_STATEMENT:
            visit(reinterpret_cast<SwitchStatement *>(statement));
            break;
        case StatementClass::WHILE_STATEMENT:
            visit(reinterpret_cast<WhileStatement *>(statement));
            break;
    }
}

void CodeGenerateVisitor::visit(Type *type) {
    switch (type->getClass()) {
        case TypeClass::ARRAY_TYPE:
            visit(reinterpret_cast<ArrayType *>(type));
            break;
        case TypeClass::FUNCTION_TYPE:
            visit(reinterpret_cast<FunctionType *>(type));
            break;
        case TypeClass::POINTER_TYPE:
            visit(reinterpret_cast<PointerType *>(type));
            break;
        case TypeClass::SCALAR_TYPE:
            visit(reinterpret_cast<ScalarType *>(type));
            break;
    }
}

void CodeGenerateVisitor::visit(BinaryExpression *binaryExpression) {
    BinaryDataType leftBinaryDataType = type2BinaryDataType(binaryExpression->leftOperand->resultType);
    BinaryDataType rightBinaryDataType = type2BinaryDataType(binaryExpression->rightOperand->resultType);
    BinaryDataType resultBinaryDataType = type2BinaryDataType(binaryExpression->resultType);
    switch (binaryExpression->binaryOperator) {
        case BinaryOperator::SUBSCRIPT: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, BinaryDataType::U64);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(binaryExpression->resultType)));
            instructionSequenceBuilder->appendMul(BinaryDataType::U64);
            instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->resultType));
            }
            break;
        }
        case BinaryOperator::MUL:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, resultBinaryDataType);
            instructionSequenceBuilder->appendMul(resultBinaryDataType);
            break;
        case BinaryOperator::DIV:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, resultBinaryDataType);
            instructionSequenceBuilder->appendDiv(resultBinaryDataType);
            break;
        case BinaryOperator::ADD:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            if (binaryExpression->rightOperand->resultType->getClass() == TypeClass::POINTER_TYPE || binaryExpression->rightOperand->resultType->getClass() == TypeClass::ARRAY_TYPE) {
                instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(binaryExpression->resultType)->sourceType)));
                instructionSequenceBuilder->appendMul(BinaryDataType::U64);
            } else {
                instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            }
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            if (binaryExpression->leftOperand->resultType->getClass() == TypeClass::POINTER_TYPE || binaryExpression->leftOperand->resultType->getClass() == TypeClass::ARRAY_TYPE) {
                instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(binaryExpression->resultType)->sourceType)));
                instructionSequenceBuilder->appendMul(BinaryDataType::U64);
            } else {
                instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            }
            instructionSequenceBuilder->appendAdd(resultBinaryDataType);
            break;
        case BinaryOperator::SUB:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            if (binaryExpression->rightOperand->resultType->getClass() == TypeClass::POINTER_TYPE || binaryExpression->rightOperand->resultType->getClass() == TypeClass::ARRAY_TYPE) {
                instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(binaryExpression->resultType)->sourceType)));
                instructionSequenceBuilder->appendMul(BinaryDataType::U64);
            } else {
                instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            }
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            if (binaryExpression->leftOperand->resultType->getClass() == TypeClass::POINTER_TYPE || binaryExpression->leftOperand->resultType->getClass() == TypeClass::ARRAY_TYPE) {
                instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(binaryExpression->resultType)->sourceType)));
                instructionSequenceBuilder->appendMul(BinaryDataType::U64);
            } else {
                instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            }
            instructionSequenceBuilder->appendSub(resultBinaryDataType);
            break;
        case BinaryOperator::MOD:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, resultBinaryDataType);
            instructionSequenceBuilder->appendMod(resultBinaryDataType);
            break;
        case BinaryOperator::SHIFT_LEFT:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
            instructionSequenceBuilder->appendSl(resultBinaryDataType);
            break;
        case BinaryOperator::SHIFT_RIGHT:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
            instructionSequenceBuilder->appendSr(resultBinaryDataType);
            break;
        case BinaryOperator::BITWISE_AND:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendAnd();
            break;
        case BinaryOperator::BITWISE_XOR:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendXor();
            break;
        case BinaryOperator::BITWISE_OR:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendOr();
            break;
        case BinaryOperator::LESS:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            instructionSequenceBuilder->appendLt(resultBinaryDataType);
            break;
        case BinaryOperator::GREATER:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            instructionSequenceBuilder->appendGt(resultBinaryDataType);
            break;
        case BinaryOperator::LESS_EQUAL:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            instructionSequenceBuilder->appendGt(resultBinaryDataType);
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
            instructionSequenceBuilder->appendXor();
            break;
        case BinaryOperator::GREATER_EQUAL:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            instructionSequenceBuilder->appendLt(resultBinaryDataType);
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
            instructionSequenceBuilder->appendXor();
            break;
        case BinaryOperator::EQUAL:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            instructionSequenceBuilder->appendEq(resultBinaryDataType);
            break;
        case BinaryOperator::NOT_EQUAL:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCast(leftBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, getLagerBinaryDataType(leftBinaryDataType, rightBinaryDataType));
            instructionSequenceBuilder->appendEq(resultBinaryDataType);
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
            instructionSequenceBuilder->appendXor();
            break;
        case BinaryOperator::LOGICAL_AND:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendTb();
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendTb();
            instructionSequenceBuilder->appendAnd();
            break;
        case BinaryOperator::LOGICAL_OR:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendTb();
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendTb();
            instructionSequenceBuilder->appendOr();
            break;
        case BinaryOperator::ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::MUL_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, leftBinaryDataType);
            instructionSequenceBuilder->appendMul(leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::DIV_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, leftBinaryDataType);
            instructionSequenceBuilder->appendDiv(leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::ADD_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            if (binaryExpression->leftOperand->resultType->getClass() == TypeClass::POINTER_TYPE) {
                instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(binaryExpression->resultType)->sourceType)));
                instructionSequenceBuilder->appendMul(BinaryDataType::U64);
            } else {
                instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            }
            instructionSequenceBuilder->appendAdd(leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::SUB_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            if (binaryExpression->leftOperand->resultType->getClass() == TypeClass::POINTER_TYPE) {
                instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(binaryExpression->resultType)->sourceType)));
                instructionSequenceBuilder->appendMul(BinaryDataType::U64);
            } else {
                instructionSequenceBuilder->appendCast(leftBinaryDataType, resultBinaryDataType);
            }
            instructionSequenceBuilder->appendSub(leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::MOD_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, leftBinaryDataType);
            instructionSequenceBuilder->appendMod(leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::SHIFT_LEFT_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
            instructionSequenceBuilder->appendSl(leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::SHIFT_RIGHT_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, BinaryDataType::U64);
            instructionSequenceBuilder->appendSr(leftBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::BITWISE_AND_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendAnd();
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::BITWISE_XOR_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendXor();
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::BITWISE_OR_ASSIGN: {
            bool originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, leftBinaryDataType);
            instructionSequenceBuilder->appendOr();
            instructionSequenceBuilder->appendStore(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(binaryExpression->leftOperand->resultType));
            }
            break;
        }
        case BinaryOperator::COMMA:
            needLoadValue = true;
            visit(binaryExpression->leftOperand);
            instructionSequenceBuilder->appendPop();
            needLoadValue = true;
            visit(binaryExpression->rightOperand);
            break;
    }
}

void CodeGenerateVisitor::visit(CallExpression *callExpression) {
    FunctionType *functionType;
    if (callExpression->functionAddress->resultType->getClass() == TypeClass::FUNCTION_TYPE) {
        functionType = reinterpret_cast<FunctionType *>(callExpression->functionAddress->resultType);
    } else {
        functionType = reinterpret_cast<FunctionType *>(reinterpret_cast<PointerType *>(callExpression->functionAddress->resultType)->sourceType);
    }
    for (int i = 0; i < functionType->parameterTypeList.size(); i++) {
        needLoadValue = true;
        visit(callExpression->argumentList[i]);
        instructionSequenceBuilder->appendCast(type2BinaryDataType(callExpression->argumentList[i]->resultType), type2BinaryDataType(functionType->parameterTypeList[i]));
    }
    needLoadValue = true;
    visit(callExpression->functionAddress);
    instructionSequenceBuilder->appendCall();
    if (functionType->returnType->getClass() == TypeClass::SCALAR_TYPE && reinterpret_cast<ScalarType *>(functionType->returnType)->baseType == BaseType::VOID) {
        instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0)); // 用于保证所有的表达式计算后都会在栈中压入一个结果，
    }
}

void CodeGenerateVisitor::visit(CastExpression *castExpression) {
    needLoadValue = true;
    visit(castExpression->operand);
    instructionSequenceBuilder->appendCast(type2BinaryDataType(castExpression->operand->resultType), type2BinaryDataType(castExpression->resultType));
}

void CodeGenerateVisitor::visit(CharacterLiteralExpression *characterLiteralExpression) {
    instructionSequenceBuilder->appendPush((std::int8_t) characterLiteralExpression->value);
}

void CodeGenerateVisitor::visit(FloatingPointLiteralExpression *floatingPointLiteralExpression) {
    instructionSequenceBuilder->appendPush((double) floatingPointLiteralExpression->value);
}

void CodeGenerateVisitor::visit(IdentifierExpression *identifierExpression) {
    Symbol *symbol = (*symbolTableIterator)[identifierExpression->identifier];
    switch (symbol->getClass()) {
        case SymbolClass::SCALAR_SYMBOL:
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<ScalarSymbol *>(symbol)->address));
            if (!symbolTable->checkGlobal(identifierExpression->identifier)) {
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
            }
            if (needLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(reinterpret_cast<ScalarSymbol *>(symbol)->type));
            }
            break;
        case SymbolClass::POINTER_SYMBOL:
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<PointerSymbol *>(symbol)->address));
            if (!symbolTable->checkGlobal(identifierExpression->identifier)) {
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
            }
            if (needLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(reinterpret_cast<PointerSymbol *>(symbol)->type));
            }
            break;
        case SymbolClass::ARRAY_SYMBOL:
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<ArraySymbol *>(symbol)->address));
            if (!symbolTable->checkGlobal(identifierExpression->identifier)) {
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
            }
            break;
        case SymbolClass::FUNCTION_SYMBOL:
            if (reinterpret_cast<FunctionSymbol *>(symbol)->address == 0) {
                functionPlaceholderIndexMap[identifierExpression->identifier].push_back(instructionSequenceBuilder->getNextInstructionIndex());
            }
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<FunctionSymbol *>(symbol)->address));
            break;
        case SymbolClass::STATEMENT_SYMBOL:
            if (reinterpret_cast<StatementSymbol *>(symbol)->address == 0) {
                statementPlaceholderIndexMap[identifierExpression->identifier].push_back(instructionSequenceBuilder->getNextInstructionIndex());
            }
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<StatementSymbol *>(symbol)->address));
            break;
    }
}

void CodeGenerateVisitor::visit(IntegerLiteralExpression *integerLiteralExpression) {
    instructionSequenceBuilder->appendPush((std::int32_t) integerLiteralExpression->value);
}

void CodeGenerateVisitor::visit(StringLiteralExpression *stringLiteralExpression) {
    StringConstant *stringLiteral = (*stringConstantPool)[stringLiteralExpression->value];
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(stringLiteral->address));
}

void CodeGenerateVisitor::visit(TernaryExpression *ternaryExpression) {
    BinaryDataType middleBinaryDataType = type2BinaryDataType(ternaryExpression->middleOperand->resultType);
    BinaryDataType rightBinaryDataType = type2BinaryDataType(ternaryExpression->rightOperand->resultType);
    BinaryDataType resultBinaryDataType = type2BinaryDataType(ternaryExpression->resultType);
    switch (ternaryExpression->ternaryOperator) {
        case TernaryOperator::CONDITION: {
            needLoadValue = true;
            visit(ternaryExpression->leftOperand);
            int instructionIndex1 = instructionSequenceBuilder->getNextInstructionIndex();
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0));
            instructionSequenceBuilder->appendJz();
            needLoadValue = true;
            visit(ternaryExpression->middleOperand);
            instructionSequenceBuilder->appendCast(middleBinaryDataType, resultBinaryDataType);
            int instructionIndex2 = instructionSequenceBuilder->getNextInstructionIndex();
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0));
            instructionSequenceBuilder->appendJmp();
            instructionSequenceBuilder->modifyPush(instructionIndex1, instructionSequenceBuilder->getNextInstructionAddress());
            needLoadValue = true;
            visit(ternaryExpression->rightOperand);
            instructionSequenceBuilder->appendCast(rightBinaryDataType, resultBinaryDataType);
            instructionSequenceBuilder->modifyPush(instructionIndex2, instructionSequenceBuilder->getNextInstructionAddress());
            break;
        }
    }
}

void CodeGenerateVisitor::visit(UnaryExpression *unaryExpression) {
    BinaryDataType operandBinaryDataType = type2BinaryDataType(unaryExpression->operand->resultType);
    BinaryDataType resultBinaryDataType = type2BinaryDataType(unaryExpression->resultType);
    switch (unaryExpression->unaryOperator) {
        case UnaryOperator::PREINCREMENT: {
            int originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            if (unaryExpression->operand->resultType->getClass() == TypeClass::POINTER_TYPE) {
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(unaryExpression->operand->resultType)->sourceType)));
            } else {
                switch (operandBinaryDataType) {
                    case BinaryDataType::I8:
                    case BinaryDataType::I16:
                    case BinaryDataType::I32:
                    case BinaryDataType::I64:
                        instructionSequenceBuilder->appendPush(static_cast<std::int64_t>(1));
                        break;
                    case BinaryDataType::U8:
                    case BinaryDataType::U16:
                    case BinaryDataType::U32:
                    case BinaryDataType::U64:
                        instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
                        break;
                    case BinaryDataType::F32:
                    case BinaryDataType::F64:
                        instructionSequenceBuilder->appendPush(static_cast<double>(1));
                        break;
                }
            }
            instructionSequenceBuilder->appendAdd(operandBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(unaryExpression->operand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            }
            break;
        }
        case UnaryOperator::PREDECREMENT: {
            int originNeedLoadValue = needLoadValue;
            needLoadValue = false;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            if (unaryExpression->operand->resultType->getClass() == TypeClass::POINTER_TYPE) {
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(unaryExpression->operand->resultType)->sourceType)));
            } else {
                switch (operandBinaryDataType) {
                    case BinaryDataType::I8:
                    case BinaryDataType::I16:
                    case BinaryDataType::I32:
                    case BinaryDataType::I64:
                        instructionSequenceBuilder->appendPush(static_cast<std::int64_t>(1));
                        break;
                    case BinaryDataType::U8:
                    case BinaryDataType::U16:
                    case BinaryDataType::U32:
                    case BinaryDataType::U64:
                        instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
                        break;
                    case BinaryDataType::F32:
                    case BinaryDataType::F64:
                        instructionSequenceBuilder->appendPush(static_cast<double>(1));
                        break;
                }
            }
            instructionSequenceBuilder->appendSub(operandBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(unaryExpression->operand->resultType));
            if (originNeedLoadValue) {
                instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            }
            break;
        }
        case UnaryOperator::POSTINCREMENT: {
            needLoadValue = false;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            instructionSequenceBuilder->appendSwap();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            if (unaryExpression->operand->resultType->getClass() == TypeClass::POINTER_TYPE) {
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(unaryExpression->operand->resultType)->sourceType)));
            } else {
                switch (operandBinaryDataType) {
                    case BinaryDataType::I8:
                    case BinaryDataType::I16:
                    case BinaryDataType::I32:
                    case BinaryDataType::I64:
                        instructionSequenceBuilder->appendPush(static_cast<std::int64_t>(1));
                        break;
                    case BinaryDataType::U8:
                    case BinaryDataType::U16:
                    case BinaryDataType::U32:
                    case BinaryDataType::U64:
                        instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
                        break;
                    case BinaryDataType::F32:
                    case BinaryDataType::F64:
                        instructionSequenceBuilder->appendPush(static_cast<double>(1));
                        break;
                }
            }
            instructionSequenceBuilder->appendAdd(operandBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(unaryExpression->operand->resultType));
            break;
        }
        case UnaryOperator::POSTDECREMENT: {
            needLoadValue = false;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            instructionSequenceBuilder->appendSwap();
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->operand->resultType));
            if (unaryExpression->operand->resultType->getClass() == TypeClass::POINTER_TYPE) {
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(reinterpret_cast<PointerType *>(unaryExpression->operand->resultType)->sourceType)));
            } else {
                switch (operandBinaryDataType) {
                    case BinaryDataType::I8:
                    case BinaryDataType::I16:
                    case BinaryDataType::I32:
                    case BinaryDataType::I64:
                        instructionSequenceBuilder->appendPush(static_cast<std::int64_t>(1));
                        break;
                    case BinaryDataType::U8:
                    case BinaryDataType::U16:
                    case BinaryDataType::U32:
                    case BinaryDataType::U64:
                        instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
                        break;
                    case BinaryDataType::F32:
                    case BinaryDataType::F64:
                        instructionSequenceBuilder->appendPush(static_cast<double>(1));
                        break;
                }
            }
            instructionSequenceBuilder->appendSub(operandBinaryDataType);
            instructionSequenceBuilder->appendStore(type2BinaryDataType(unaryExpression->operand->resultType));
            break;
        }
        case UnaryOperator::TAKE_ADDRESS: {
            needLoadValue = false;
            visit(unaryExpression->operand);
            break;
        }
        case UnaryOperator::DEREFERENCE:
            // 不需要修改needLoadValue
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendLoad(type2BinaryDataType(unaryExpression->resultType));
            break;
        case UnaryOperator::PLUS:
            needLoadValue = true;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendCast(operandBinaryDataType, resultBinaryDataType);
            break;
        case UnaryOperator::MINUS:
            needLoadValue = true;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendCast(operandBinaryDataType, resultBinaryDataType);
            instructionSequenceBuilder->appendNeg(resultBinaryDataType);
            break;
        case UnaryOperator::BITWISE_NOT:
            needLoadValue = true;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendNot();
            break;
        case UnaryOperator::LOGICAL_NOT:
            needLoadValue = true;
            visit(unaryExpression->operand);
            instructionSequenceBuilder->appendTb();
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(1));
            instructionSequenceBuilder->appendXor();
            break;
        case UnaryOperator::SIZEOF:
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(getTypeByteNum(unaryExpression->operand->resultType)));
            break;
    }
}

void CodeGenerateVisitor::visit(ArrayType *arrayType) {
}

void CodeGenerateVisitor::visit(FunctionType *functionType) {
}

void CodeGenerateVisitor::visit(PointerType *pointerType) {
}

void CodeGenerateVisitor::visit(ScalarType *scalarType) {
}

void CodeGenerateVisitor::visit(FunctionDeclaration *functionDeclaration) {
}

void CodeGenerateVisitor::visit(FunctionDefinition *functionDefinition) {
    std::uint64_t functionAddress = instructionSequenceBuilder->getNextInstructionAddress();
    if (functionPlaceholderIndexMap.contains(functionDefinition->identifier)) {
        patchFunctionPlaceholderAddress(functionDefinition->identifier, functionAddress);
    }
    reinterpret_cast<FunctionSymbol *>((*symbolTableIterator)[functionDefinition->identifier])->address = functionAddress;
    symbolTableIterator->switchScope();
    for (int i = static_cast<int>(reinterpret_cast<FunctionType *>(functionDefinition->functionType)->parameterTypeList.size()) - 1; i >= 0; i--) {
        switch (reinterpret_cast<FunctionType *>(functionDefinition->functionType)->parameterTypeList[i]->getClass()) {
            case TypeClass::ARRAY_TYPE:
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<ArraySymbol *>((*symbolTableIterator)[reinterpret_cast<VariableDeclaration *>(functionDefinition->parameterDeclarationList[i])->identifier])->address));
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
                instructionSequenceBuilder->appendSwap();
                instructionSequenceBuilder->appendStore(type2BinaryDataType(reinterpret_cast<FunctionType *>(functionDefinition->functionType)->parameterTypeList[i]));
                break;
            case TypeClass::FUNCTION_TYPE:
                assert(false);
            case TypeClass::POINTER_TYPE:
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<PointerSymbol *>((*symbolTableIterator)[reinterpret_cast<VariableDeclaration *>(functionDefinition->parameterDeclarationList[i])->identifier])->address));
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
                instructionSequenceBuilder->appendSwap();
                instructionSequenceBuilder->appendStore(type2BinaryDataType(reinterpret_cast<FunctionType *>(functionDefinition->functionType)->parameterTypeList[i]));
                break;
            case TypeClass::SCALAR_TYPE:
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(reinterpret_cast<ScalarSymbol *>((*symbolTableIterator)[reinterpret_cast<VariableDeclaration *>(functionDefinition->parameterDeclarationList[i])->identifier])->address));
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
                instructionSequenceBuilder->appendSwap();
                instructionSequenceBuilder->appendStore(type2BinaryDataType(reinterpret_cast<FunctionType *>(functionDefinition->functionType)->parameterTypeList[i]));
                break;
        }
    }
    visit(functionDefinition->body);
    if (instructionSequenceBuilder->getLastInstruction().opcode != Opcode::RET) {
        instructionSequenceBuilder->appendRet();
    }
    symbolTableIterator->switchScope();
}

void CodeGenerateVisitor::visit(VariableDeclaration *variableDeclaration) {
    switch (variableDeclaration->variableType->getClass()) {
        case TypeClass::ARRAY_TYPE: {
            auto arraySymbol = reinterpret_cast<ArraySymbol *>((*symbolTableIterator)[variableDeclaration->identifier]);
            if (!variableDeclaration->initialValueList.empty()) {
                for (int i = 0; i < variableDeclaration->initialValueList.size(); i++) {
                    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(arraySymbol->address));
                    instructionSequenceBuilder->appendFbp();
                    instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
                    instructionSequenceBuilder->appendPush(getTypeByteNum(reinterpret_cast<ArrayType *>(variableDeclaration->variableType)->elemType) * i);
                    instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
                    needLoadValue = true;
                    visit(variableDeclaration->initialValueList[i]);
                    instructionSequenceBuilder->appendStore(type2BinaryDataType(variableDeclaration->variableType));
                }
            }
            break;
        }
        case TypeClass::FUNCTION_TYPE:
            assert(false);
        case TypeClass::POINTER_TYPE: {
            auto pointerSymbol = reinterpret_cast<PointerSymbol *>((*symbolTableIterator)[variableDeclaration->identifier]);
            if (!variableDeclaration->initialValueList.empty()) {
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(pointerSymbol->address));
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
                needLoadValue = true;
                visit(variableDeclaration->initialValueList[0]);
                instructionSequenceBuilder->appendStore(type2BinaryDataType(variableDeclaration->variableType));
            }
            break;
        }
        case TypeClass::SCALAR_TYPE: {
            auto scalarSymbol = reinterpret_cast<ScalarSymbol *>((*symbolTableIterator)[variableDeclaration->identifier]);
            if (!variableDeclaration->initialValueList.empty()) {
                instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(scalarSymbol->address));
                instructionSequenceBuilder->appendFbp();
                instructionSequenceBuilder->appendAdd(BinaryDataType::U64);
                needLoadValue = true;
                visit(variableDeclaration->initialValueList[0]);
                instructionSequenceBuilder->appendStore(type2BinaryDataType(variableDeclaration->variableType));
            }
            break;
        }
    }
}

void CodeGenerateVisitor::visit(BreakStatement *breakStatement) {
    breakPushIndexListStack.top().push_back(instructionSequenceBuilder->getNextInstructionIndex());
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0)); // 占位地址，需要后续for、while、do-while语句确定地址后再进行修改
    instructionSequenceBuilder->appendJmp();
}

void CodeGenerateVisitor::visit(CaseStatement *caseStatement) {
    // 对switch语句设置的占位地址进行修改
    instructionSequenceBuilder->modifyPush(switchPushIndexListStack.top().front(), instructionSequenceBuilder->getNextInstructionAddress());
    switchPushIndexListStack.top().pop_back();
    visit(caseStatement->statement);
}

void CodeGenerateVisitor::visit(CompoundStatement *compoundStatement) {
    symbolTableIterator->switchScope();
    for (auto statement : compoundStatement->statementList) {
        visit(statement);
    }
    symbolTableIterator->switchScope();
}

void CodeGenerateVisitor::visit(ContinueStatement *continueStatement) {
    continuePushIndexListStack.top().push_back(instructionSequenceBuilder->getNextInstructionIndex());
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(continueJumpAddressStack.top())); // 这个不一定是真实的跳转地址，当位于for和while循环时才是真实地址，当位于do-while循环时则是占位地址
    instructionSequenceBuilder->appendJmp();
}

void CodeGenerateVisitor::visit(DeclarationStatement *declarationStatement) {
    for (auto declaration : declarationStatement->declarationList) {
        visit(declaration);
    }
}

void CodeGenerateVisitor::visit(DefaultStatement *defaultStatement) {
    // 对switch语句设置的占位地址进行修改
    instructionSequenceBuilder->modifyPush(switchPushIndexListStack.top().front(), instructionSequenceBuilder->getNextInstructionAddress());
    switchPushIndexListStack.top().pop_back();
    visit(defaultStatement->statement);
}

void CodeGenerateVisitor::visit(DoWhileStatement *doWhileStatement) {
    continueJumpAddressStack.push(static_cast<std::uint64_t>(0)); // 占位地址
    breakPushIndexListStack.emplace();
    continuePushIndexListStack.emplace();
    std::uint64_t jumpAddress1 = instructionSequenceBuilder->getNextInstructionAddress();
    visit(doWhileStatement->body);
    patchContinuePushAddress(instructionSequenceBuilder->getNextInstructionAddress());
    needLoadValue = true;
    visit(doWhileStatement->condition);
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(jumpAddress1));
    instructionSequenceBuilder->appendJnz();
    patchBreakPushAddress(instructionSequenceBuilder->getNextInstructionAddress());
    continuePushIndexListStack.pop();
    breakPushIndexListStack.pop();
    continueJumpAddressStack.pop();
}

void CodeGenerateVisitor::visit(ExpressionStatement *expressionStatement) {
    if (expressionStatement->expression != nullptr) {
        needLoadValue = true;
        visit(expressionStatement->expression);
        instructionSequenceBuilder->appendPop(); // 表达式的值没有用
    }
}

void CodeGenerateVisitor::visit(ForStatement *forStatement) {
    symbolTableIterator->switchScope();
    // 初始化部分
    for (auto initDeclaration : forStatement->declarationList) {
        visit(initDeclaration);
    }
    if (forStatement->init != nullptr) {
        needLoadValue = true;
        visit(forStatement->init);
        instructionSequenceBuilder->appendPop(); // 表达式的值没有用
    }
    // 循环部分
    std::uint64_t jumpAddress1 = instructionSequenceBuilder->getNextInstructionAddress();
    continueJumpAddressStack.push(jumpAddress1);
    breakPushIndexListStack.emplace();
    continuePushIndexListStack.emplace();
    if (forStatement->condition != nullptr) {
        needLoadValue = true;
        visit(forStatement->condition);
    }
    int instructionIndex1 = instructionSequenceBuilder->getNextInstructionIndex();
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0));
    if (forStatement->condition != nullptr) {
        instructionSequenceBuilder->appendJz();
    } else {
        instructionSequenceBuilder->appendJmp();
    }
    visit(forStatement->body);
    if (forStatement->update != nullptr) {
        needLoadValue = true;
        visit(forStatement->update);
        instructionSequenceBuilder->appendPop(); // 表达式的值没有用
    }
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(jumpAddress1));
    instructionSequenceBuilder->appendJmp();
    instructionSequenceBuilder->modifyPush(instructionIndex1, instructionSequenceBuilder->getNextInstructionAddress());
    patchBreakPushAddress(instructionSequenceBuilder->getNextInstructionAddress());
    continuePushIndexListStack.pop();
    breakPushIndexListStack.pop();
    continueJumpAddressStack.pop();
    symbolTableIterator->switchScope();
}

void CodeGenerateVisitor::visit(GotoStatement *gotoStatement) {
    auto statementSymbol = reinterpret_cast<StatementSymbol *>((*symbolTableIterator)[gotoStatement->identifier]);
    if (statementSymbol->address == 0) {
        statementPlaceholderIndexMap[gotoStatement->identifier].push_back(instructionSequenceBuilder->getNextInstructionIndex());
    }
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(statementSymbol->address));
    instructionSequenceBuilder->appendJmp();
}

void CodeGenerateVisitor::visit(IfStatement *ifStatement) {
    needLoadValue = true;
    visit(ifStatement->condition);
    int instructionIndex1 = instructionSequenceBuilder->getNextInstructionIndex();
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0)); // 占位
    instructionSequenceBuilder->appendJz();
    visit(ifStatement->trueBody);
    int instructionIndex2;
    if (ifStatement->falseBody != nullptr) {
        instructionIndex2 = instructionSequenceBuilder->getNextInstructionIndex();
        instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0)); // 占位
        instructionSequenceBuilder->appendJmp();
    }
    instructionSequenceBuilder->modifyPush(instructionIndex1, instructionSequenceBuilder->getNextInstructionAddress());
    if (ifStatement->falseBody != nullptr) {
        visit(ifStatement->falseBody);
        instructionSequenceBuilder->modifyPush(instructionIndex2, instructionSequenceBuilder->getNextInstructionAddress());
    }
}

void CodeGenerateVisitor::visit(LabelStatement *labelStatement) {
    std::uint64_t statementAddress = instructionSequenceBuilder->getNextInstructionAddress();
    if (statementPlaceholderIndexMap.contains(labelStatement->identifier)) {
        patchStatementPlaceholderAddress(labelStatement->identifier, statementAddress);
    }
    visit(labelStatement->statement);
}

void CodeGenerateVisitor::visit(ReturnStatement *returnStatement) {
    if (returnStatement->value != nullptr) {
        needLoadValue = true;
        visit(returnStatement->value);
    }
    instructionSequenceBuilder->appendRet();
}

void CodeGenerateVisitor::visit(SwitchStatement *switchStatement) {
    // switch语句的子语句必定是compound语句，且compound语句内必定是case或default语句
    needLoadValue = true;
    visit(switchStatement->expression);
    std::vector<Statement *> caseStatementList = reinterpret_cast<CompoundStatement *>(switchStatement->body)->statementList;
    int defaultStatementIndex;
    // 找出default语句
    for (int i = 0; i < caseStatementList.size(); i++) {
        if (caseStatementList[i]->getClass() == StatementClass::DEFAULT_STATEMENT) {
            defaultStatementIndex = i;
        }
    }
    std::vector<int> switchPushIndexList;
    // 先处理case语句
    for (int i = 0; i < caseStatementList.size() - 1; i++) {
        if (caseStatementList[i]->getClass() == StatementClass::CASE_STATEMENT) {
            instructionSequenceBuilder->appendCopy();
            instructionSequenceBuilder->appendPush(reinterpret_cast<CaseStatement *>(caseStatementList[i])->value);
            instructionSequenceBuilder->appendCast(BinaryDataType::I64, type2BinaryDataType(switchStatement->expression->resultType));
            instructionSequenceBuilder->appendEq(type2BinaryDataType(switchStatement->expression->resultType));
            switchPushIndexList[i] = instructionSequenceBuilder->getNextInstructionIndex();
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0)); // 占位跳转地址，需要后续case语句确定地址后再进行修改
            instructionSequenceBuilder->appendJnz();
        }
    }
    // 再处理default语句
    switchPushIndexList[defaultStatementIndex] = instructionSequenceBuilder->getNextInstructionIndex();
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0)); // 占位跳转地址，需要后续case语句的visit函数确定地址后再进行修改
    instructionSequenceBuilder->appendJmp();
    switchPushIndexListStack.push(switchPushIndexList);
    breakPushIndexListStack.emplace();
    visit(switchStatement->body);
    patchBreakPushAddress(instructionSequenceBuilder->getNextInstructionAddress());
    breakPushIndexListStack.pop();
    switchPushIndexListStack.pop();
    instructionSequenceBuilder->appendPop(); // 弹出一开始压入的条件表达式值
}

void CodeGenerateVisitor::visit(WhileStatement *whileStatement) {
    std::uint64_t jumpAddress1 = instructionSequenceBuilder->getNextInstructionAddress();
    continueJumpAddressStack.push(jumpAddress1);
    breakPushIndexListStack.emplace();
    continuePushIndexListStack.emplace();
    needLoadValue = true;
    visit(whileStatement->condition);
    int instructionIndex1 = instructionSequenceBuilder->getNextInstructionIndex();
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0));
    instructionSequenceBuilder->appendJz();
    visit(whileStatement->body);
    instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(jumpAddress1));
    instructionSequenceBuilder->appendJmp();
    instructionSequenceBuilder->modifyPush(instructionIndex1, instructionSequenceBuilder->getNextInstructionAddress());
    patchBreakPushAddress(instructionSequenceBuilder->getNextInstructionAddress());
    continuePushIndexListStack.pop();
    breakPushIndexListStack.pop();
    continueJumpAddressStack.pop();
}

void CodeGenerateVisitor::visit(TranslationUnit *translationUnit) {
    std::deque<Declaration *> declarationDeque;
    for (auto declaration : translationUnit->declarationList) {
        if (declaration->getClass() == DeclarationClass::FUNCTION_DEFINITION) {
            declarationDeque.push_back(declaration);
        } else {
            declarationDeque.push_front(declaration);
        }
    }
    translationUnit->declarationList = std::vector<Declaration *>(declarationDeque.begin(), declarationDeque.end());
    bool beginFunctionDefinition = false;
    for (auto declaration : translationUnit->declarationList) {
        if (!beginFunctionDefinition && declaration->getClass() == DeclarationClass::FUNCTION_DEFINITION) {
            beginFunctionDefinition = true;
            functionPlaceholderIndexMap["main"].push_back(instructionSequenceBuilder->getNextInstructionIndex());
            instructionSequenceBuilder->appendPush(static_cast<std::uint64_t>(0)); // 占位地址
            instructionSequenceBuilder->appendCall();
            instructionSequenceBuilder->appendHlt();
            BuiltInFunctionInserter::insertCode(symbolTableIterator, instructionSequenceBuilder);
        }
        visit(declaration);
    }
}

InstructionSequence *CodeGenerateVisitor::generateCode(TranslationUnit *translationUnit, SymbolTable *symbolTable, StringConstantPool *stringConstantPool) {
    auto *codeGenerateVisitor = new CodeGenerateVisitor(symbolTable, stringConstantPool);
    translationUnit->accept(codeGenerateVisitor);
    InstructionSequence *instructionSequence = codeGenerateVisitor->instructionSequenceBuilder->build();
    delete codeGenerateVisitor;
    return instructionSequence;
}


