#include "ErrorCheckVisitor.h"

#include <algorithm>
#include <map>
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
#include "../../symbol/ArraySymbol.h"
#include "../../symbol/FunctionSymbol.h"
#include "../../symbol/PointerSymbol.h"
#include "../../symbol/StatementSymbol.h"
#include "../../symbol/ScalarSymbol.h"
#include "../../error/ErrorHandler.h"
#include "../../builtin/BuiltInFunctionLibrary.h"

bool isScalarType(Type *type) {
    return type->getClass() == TypeClass::SCALAR_TYPE;
}

bool isArrayType(Type *type) {
    return type->getClass() == TypeClass::ARRAY_TYPE;
}

bool isPointerType(Type *type) {
    return type->getClass() == TypeClass::POINTER_TYPE;
}

bool isFunctionType(Type *type) {
    return type->getClass() == TypeClass::FUNCTION_TYPE;
}

bool isVoidScalarType(Type *type) {
    return type->getClass() == TypeClass::SCALAR_TYPE && ((ScalarType *) type)->baseType == BaseType::VOID;
}

bool isVoidArrayType(Type *type) {
    return type->getClass() == TypeClass::ARRAY_TYPE && ((ArrayType *) type)->elemType->getClass() == TypeClass::SCALAR_TYPE && ((ScalarType *) ((ArrayType *) type)->elemType)->baseType == BaseType::VOID;
}

bool isVoidPointerType(Type *type) {
    return type->getClass() == TypeClass::POINTER_TYPE && ((PointerType *) type)->sourceType->getClass() == TypeClass::SCALAR_TYPE && ((ScalarType *) ((PointerType *) type)->sourceType)->baseType == BaseType::VOID;
}

bool isVoidFunctionType(Type *type) {
    return type->getClass() == TypeClass::FUNCTION_TYPE && ((FunctionType *) type)->returnType->getClass() == TypeClass::SCALAR_TYPE && ((ScalarType *) ((FunctionType *) type)->returnType)->baseType == BaseType::VOID;
}

bool isIntegerScalarType(Type *type) {
    return type->getClass() == TypeClass::SCALAR_TYPE && ((ScalarType *) type)->baseType != BaseType::VOID && ((ScalarType *) type)->baseType != BaseType::FLOAT && ((ScalarType *) type)->baseType != BaseType::DOUBLE;
}

bool isCharPointerType(Type *type) {
    return type->getClass() == TypeClass::POINTER_TYPE && ((PointerType *) type)->sourceType->getClass() == TypeClass::SCALAR_TYPE && ((ScalarType *) ((PointerType *) type)->sourceType)->baseType == BaseType::CHAR;
}

bool isSameType(Type *type1, Type *type2) {
    if (type1->getClass() != type2->getClass()) {
        return false;
    }
    switch (type1->getClass()) {
        case TypeClass::ARRAY_TYPE: {
            auto *arrayType1 = (ArrayType *) type1;
            auto *arrayType2 = (ArrayType *) type2;
            if (!isSameType(arrayType1->elemType, arrayType2->elemType)) {
                return false;
            }
            if (arrayType1->size != arrayType2->size) {
                return false;
            }
            return true;
        }
        case TypeClass::SCALAR_TYPE: {
            auto *scalarType1 = (ScalarType *) type1;
            auto *scalarType2 = (ScalarType *) type2;
            if (scalarType1->baseType != scalarType2->baseType) {
                return false;
            }
            return true;
        }
        case TypeClass::FUNCTION_TYPE: {
            auto *functionType1 = (FunctionType *) type1;
            auto *functionType2 = (FunctionType *) type2;
            if (!isSameType(functionType1->returnType, functionType2->returnType)) {
                return false;
            }
            if (functionType1->parameterTypeList.size() != functionType2->parameterTypeList.size()) {
                return false;
            }
            for (int i = 0; i < functionType1->parameterTypeList.size(); i++) {
                if (!isSameType(functionType1->parameterTypeList[i], functionType2->parameterTypeList[i])) {
                    return false;
                }
            }
            return true;
        }
        case TypeClass::POINTER_TYPE: {
            auto *pointerType1 = (PointerType *) type1;
            auto *pointerType2 = (PointerType *) type2;
            if (!isSameType(pointerType1->sourceType, pointerType2->sourceType)) {
                return false;
            }
            return true;
        }
    }
    assert(false);
}

int getTypeImplicitCastPriority(Type *type) {
    switch (type->getClass()) {
        case TypeClass::ARRAY_TYPE:
            return 10;
        case TypeClass::FUNCTION_TYPE:
            return 9;
        case TypeClass::POINTER_TYPE:
            return 11;
        case TypeClass::SCALAR_TYPE:
            switch (((ScalarType *) type)->baseType) {
                case BaseType::VOID:
                    assert(false);
                case BaseType::CHAR:
                    return 1;
                case BaseType::UNSIGNED_CHAR:
                    return 2;
                case BaseType::SHORT:
                    return 3;
                case BaseType::UNSIGNED_SHORT:
                    return 4;
                case BaseType::INT:
                case BaseType::LONG_INT:
                    return 5;
                case BaseType::UNSIGNED_INT:
                case BaseType::UNSIGNED_LONG_INT:
                    return 6;
                case BaseType::LONG_LONG_INT:
                    return 7;
                case BaseType::UNSIGNED_LONG_LONG_INT:
                    return 8;
                case BaseType::FLOAT:
                    return 12;
                case BaseType::DOUBLE:
                    return 13;
            }
    }
    assert(false);
}

bool canImplicitCastOneWay(Type *sourceType, Type *targetType) {
    if (isSameType(sourceType, targetType)) {
        return true;
    }
    switch (sourceType->getClass()) {
        case TypeClass::ARRAY_TYPE:
            return isPointerType(targetType) && isSameType(((ArrayType *) sourceType)->elemType, ((PointerType *) targetType)->sourceType);
        case TypeClass::FUNCTION_TYPE:
            return isPointerType(targetType) && isSameType(sourceType, ((PointerType *) targetType)->sourceType);
        case TypeClass::POINTER_TYPE:
            return isVoidPointerType(targetType);
        case TypeClass::SCALAR_TYPE:
            switch (((ScalarType *) sourceType)->baseType) {
                case BaseType::VOID:
                    return false;
                case BaseType::CHAR:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_CHAR ||
                               ((ScalarType *) targetType)->baseType == BaseType::SHORT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_SHORT ||
                               ((ScalarType *) targetType)->baseType == BaseType::INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::SHORT:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_SHORT ||
                               ((ScalarType *) targetType)->baseType == BaseType::INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::INT:
                case BaseType::LONG_INT:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::LONG_LONG_INT:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::UNSIGNED_CHAR:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::SHORT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_SHORT ||
                               ((ScalarType *) targetType)->baseType == BaseType::INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::UNSIGNED_SHORT:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::UNSIGNED_INT:
                case BaseType::UNSIGNED_LONG_INT:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::UNSIGNED_LONG_LONG_INT ||
                               ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::UNSIGNED_LONG_LONG_INT:
                    if (isPointerType(targetType) || isArrayType(targetType)) {
                        return true;
                    } else if (isScalarType(targetType)) {
                        return ((ScalarType *) targetType)->baseType == BaseType::FLOAT ||
                               ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                    } else {
                        return false;
                    }
                case BaseType::FLOAT:
                    return isScalarType(targetType) && ((ScalarType *) targetType)->baseType == BaseType::DOUBLE;
                case BaseType::DOUBLE:
                    return false;
            }
    }
    if (isVoidScalarType(sourceType) || isVoidScalarType(targetType)) {
        return isVoidScalarType(sourceType) && isVoidScalarType(targetType);
    } else {
        return getTypeImplicitCastPriority(sourceType) <= getTypeImplicitCastPriority(targetType);
    }
}

bool canImplicitCastTwoWay(Type *type1, Type *type2) {
    return canImplicitCastOneWay(type1, type2) || canImplicitCastOneWay(type2, type1);
}

Type *implicitCastTwoWay(Type *type1, Type *type2) {
    if (canImplicitCastOneWay(type1, type2)) {
        return type2;
    } else if (canImplicitCastOneWay(type2, type1)) {
        return type1;
    }
    assert(false);
}

bool haveConstTypeQualifier(Type *type) {
    if (type->getClass() == TypeClass::SCALAR_TYPE) {
        return std::any_of(((ScalarType *) type)->typeQualifierList.begin(), ((ScalarType *) type)->typeQualifierList.end(), [](const TypeQualifier &typeQualifier) {
            return typeQualifier == TypeQualifier::CONST;
        });
    }
    if (type->getClass() == TypeClass::POINTER_TYPE) {
        return std::any_of(((PointerType *) type)->typeQualifierList.begin(), ((PointerType *) type)->typeQualifierList.end(), [](const TypeQualifier &typeQualifier) {
            return typeQualifier == TypeQualifier::CONST;
        });
    }
    return false;
}

void ErrorCheckVisitor::visit(Declaration *declaration) {
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

void ErrorCheckVisitor::visit(Expression *expression) {
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

void ErrorCheckVisitor::visit(Statement *statement) {
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

void ErrorCheckVisitor::visit(Type *type) {
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

void ErrorCheckVisitor::visit(BinaryExpression *binaryExpression) {
    visit(binaryExpression->leftOperand);
    if (ErrorHandler::getStatus()) return;
    visit(binaryExpression->rightOperand);
    if (ErrorHandler::getStatus()) return;
    if (isVoidScalarType(binaryExpression->leftOperand->resultType) || isVoidScalarType(binaryExpression->rightOperand->resultType)) {
        ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "void scalar can't participate in calculation");
        return;
    }
    switch (binaryExpression->binaryOperator) {
        case BinaryOperator::SUBSCRIPT:
            if (!isIntegerScalarType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the right operand of the subscript operator must be an integer scalar");
                return;
            }
            if (isArrayType(binaryExpression->leftOperand->resultType)) {
                if (isVoidScalarType(((ArrayType *) binaryExpression->leftOperand->resultType)->elemType)) {
                    ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the subscript operator can‘t be a void array");
                    return;
                }
                binaryExpression->isLvalue = true;
                binaryExpression->resultType = ((ArrayType *) binaryExpression->leftOperand->resultType)->elemType->clone();
            } else if (isPointerType(binaryExpression->leftOperand->resultType)) {
                if (isVoidScalarType(((PointerType *) binaryExpression->leftOperand->resultType)->sourceType)) {
                    ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the subscript operator can‘t be a void pointer");
                    return;
                }
                binaryExpression->isLvalue = true;
                binaryExpression->resultType = ((PointerType *) binaryExpression->leftOperand->resultType)->sourceType->clone();
            } else {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the subscript operator must be an array or a pointer");
                return;
            }
            break;
        case BinaryOperator::MUL:
        case BinaryOperator::DIV:
            if (!isScalarType(binaryExpression->leftOperand->resultType) || !isScalarType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the multiply or divide operator must be a scalar");
                return;
            }
            if (!canImplicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the multiply or divide operator can't cast implicitly");
                return;
            }
            binaryExpression->resultType = implicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)->clone();
            break;
        case BinaryOperator::ADD:
        case BinaryOperator::SUB: {
            if (!((isScalarType(binaryExpression->leftOperand->resultType) && isScalarType(binaryExpression->rightOperand->resultType)) ||
                  (isPointerType(binaryExpression->leftOperand->resultType) && isIntegerScalarType(binaryExpression->rightOperand->resultType)) ||
                  (isArrayType(binaryExpression->leftOperand->resultType) && isIntegerScalarType(binaryExpression->rightOperand->resultType)) ||
                  (isIntegerScalarType(binaryExpression->leftOperand->resultType) && isPointerType(binaryExpression->rightOperand->resultType)) ||
                  (isIntegerScalarType(binaryExpression->leftOperand->resultType) && isArrayType(binaryExpression->rightOperand->resultType)))) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the add or subtract operator must be a scalar, pointer or array");
                return;
            }
            if (isVoidPointerType(binaryExpression->leftOperand->resultType) || isVoidArrayType(binaryExpression->leftOperand->resultType) || isVoidPointerType(binaryExpression->rightOperand->resultType) || isVoidArrayType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "void pointer can't participate in calculation");
                return;
            }
            if (!canImplicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the add or subtract operator can't cast implicitly");
                return;
            }
            Type *targetType = implicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType);
            if (targetType->getClass() == TypeClass::ARRAY_TYPE) {
                binaryExpression->resultType = new PointerType(-1, -1, ((ArrayType *) targetType)->elemType->clone(), {});
            } else {
                binaryExpression->resultType = targetType->clone();
            }
            break;
        }
        case BinaryOperator::MOD:
        case BinaryOperator::BITWISE_AND:
        case BinaryOperator::BITWISE_XOR:
        case BinaryOperator::BITWISE_OR:
            if (!isIntegerScalarType(binaryExpression->leftOperand->resultType) || !isIntegerScalarType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the bitwise operator must be an integer scalar");
                return;
            }
            if (!canImplicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the bitwise operator can't cast implicitly");
                return;
            }
            binaryExpression->resultType = implicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)->clone();
            break;
        case BinaryOperator::SHIFT_LEFT:
        case BinaryOperator::SHIFT_RIGHT:
            if (!isIntegerScalarType(binaryExpression->leftOperand->resultType) || !isIntegerScalarType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the modulo or shift operator must be an integer scalar");
                return;
            }
            binaryExpression->resultType = binaryExpression->leftOperand->resultType->clone();
            break;
        case BinaryOperator::LESS:
        case BinaryOperator::GREATER:
        case BinaryOperator::LESS_EQUAL:
        case BinaryOperator::GREATER_EQUAL:
        case BinaryOperator::EQUAL:
        case BinaryOperator::NOT_EQUAL:
            if (!((isScalarType(binaryExpression->leftOperand->resultType) || isPointerType(binaryExpression->leftOperand->resultType)) &&
                  (isScalarType(binaryExpression->rightOperand->resultType) || isPointerType(binaryExpression->rightOperand->resultType)))) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the logical operator must be an integer scalar or pointer");
                return;
            }
            if (!canImplicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the logical operator can't cast implicitly");
                return;
            }
            binaryExpression->resultType = new ScalarType(-1, -1, BaseType::LONG_LONG_INT, {TypeQualifier::CONST});
            break;
        case BinaryOperator::LOGICAL_AND:
        case BinaryOperator::LOGICAL_OR:
            if (!((isIntegerScalarType(binaryExpression->leftOperand->resultType) || isPointerType(binaryExpression->leftOperand->resultType)) &&
                  (isIntegerScalarType(binaryExpression->rightOperand->resultType) || isPointerType(binaryExpression->rightOperand->resultType)))) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the logical operator must be an integer scalar or pointer");
                return;
            }
            if (!canImplicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the logical operator can't cast implicitly");
                return;
            }
            binaryExpression->resultType = new ScalarType(-1, -1, BaseType::LONG_LONG_INT, {TypeQualifier::CONST});
            break;
        case BinaryOperator::ASSIGN:
            if (!(isScalarType(binaryExpression->leftOperand->resultType) || isPointerType(binaryExpression->leftOperand->resultType))) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the assign operator must be a scalar or pointer");
                return;
            }
            if (haveConstTypeQualifier(binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "can't assign because have the const qualifier");
                return;
            }
            if (!binaryExpression->leftOperand->isLvalue) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the assign operator must be a lvalue");
                return;
            }
            if (!canImplicitCastOneWay(binaryExpression->rightOperand->resultType, binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the assign operator can't cast implicitly");
                return;
            }
            binaryExpression->isLvalue = true;
            binaryExpression->resultType = binaryExpression->leftOperand->resultType->clone();
            break;
        case BinaryOperator::MUL_ASSIGN:
        case BinaryOperator::DIV_ASSIGN:
            if (!isScalarType(binaryExpression->leftOperand->resultType) || !isScalarType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the multiply or divide operator must be a scalar");
                return;
            }
            if (haveConstTypeQualifier(binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "can't assign because have the const qualifier");
                return;
            }
            if (!binaryExpression->leftOperand->isLvalue) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the assign operator must be a lvalue");
                return;
            }
            if (!canImplicitCastOneWay(binaryExpression->rightOperand->resultType, binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the assign operator can't cast implicitly");
                return;
            }
            binaryExpression->isLvalue = true;
            binaryExpression->resultType = binaryExpression->leftOperand->resultType->clone();
        case BinaryOperator::ADD_ASSIGN:
        case BinaryOperator::SUB_ASSIGN:
            if (!((isScalarType(binaryExpression->leftOperand->resultType) || isPointerType(binaryExpression->leftOperand->resultType)) &&
                  (isScalarType(binaryExpression->rightOperand->resultType)))) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the add or subtract operator must be a scalar, pointer or array");
                return;
            }
            if (isVoidPointerType(binaryExpression->leftOperand->resultType) || isVoidPointerType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "void pointer can't participate in calculation");
                return;
            }
            if (haveConstTypeQualifier(binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "can't assign because have the const qualifier");
                return;
            }
            if (!binaryExpression->leftOperand->isLvalue) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the assign operator must be a lvalue");
                return;
            }
            if (!canImplicitCastOneWay(binaryExpression->rightOperand->resultType, binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the assign operator can't cast implicitly");
                return;
            }
            binaryExpression->isLvalue = true;
            binaryExpression->resultType = binaryExpression->leftOperand->resultType->clone();
            break;
        case BinaryOperator::MOD_ASSIGN:
        case BinaryOperator::BITWISE_AND_ASSIGN:
        case BinaryOperator::BITWISE_XOR_ASSIGN:
        case BinaryOperator::BITWISE_OR_ASSIGN:
            if (!isIntegerScalarType(binaryExpression->leftOperand->resultType) || !isIntegerScalarType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the multiply, divide, modulo or bitwise operator must be an integer scalar");
                return;
            }
            if (haveConstTypeQualifier(binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "can't assign because have the const qualifier");
                return;
            }
            if (!binaryExpression->leftOperand->isLvalue) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the assign operator must be a lvalue");
                return;
            }
            if (!canImplicitCastOneWay(binaryExpression->rightOperand->resultType, binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the assign operator can't cast implicitly");
                return;
            }
            binaryExpression->isLvalue = true;
            binaryExpression->resultType = implicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)->clone();
            break;
        case BinaryOperator::SHIFT_LEFT_ASSIGN:
        case BinaryOperator::SHIFT_RIGHT_ASSIGN:
            if (!isIntegerScalarType(binaryExpression->leftOperand->resultType) || !isIntegerScalarType(binaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the operand of the multiply, divide, modulo or bitwise operator must be an integer scalar");
                return;
            }
            if (haveConstTypeQualifier(binaryExpression->leftOperand->resultType)) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "can't assign because have the const qualifier");
                return;
            }
            if (!binaryExpression->leftOperand->isLvalue) {
                ErrorHandler::error(binaryExpression->lineNumber, binaryExpression->columnNumber, "the left operand of the assign operator must be a lvalue");
                return;
            }
            binaryExpression->isLvalue = true;
            binaryExpression->resultType = implicitCastTwoWay(binaryExpression->leftOperand->resultType, binaryExpression->rightOperand->resultType)->clone();
            break;
        case BinaryOperator::COMMA:
            binaryExpression->resultType = binaryExpression->rightOperand->resultType->clone();
            break;
    }
}

void ErrorCheckVisitor::visit(CallExpression *callExpression) {
    visit(callExpression->functionAddress);
    if (ErrorHandler::getStatus()) return;
    FunctionType *functionType;
    if (isFunctionType(callExpression->functionAddress->resultType)) {
        functionType = (FunctionType *) callExpression->functionAddress->resultType;
    } else if (isPointerType(callExpression->functionAddress->resultType) && isFunctionType(((PointerType *) callExpression->functionAddress->resultType)->sourceType)) {
        functionType = (FunctionType *)((PointerType *)callExpression->functionAddress->resultType)->sourceType;
    } else {
        ErrorHandler::error(callExpression->lineNumber, callExpression->columnNumber, "the left operand of the call operator must be a function");
        return;
    }
    std::vector<Type *> parameterTypeList = functionType->parameterTypeList;
    if (parameterTypeList.size() != callExpression->argumentList.size()) {
        ErrorHandler::error(callExpression->lineNumber, callExpression->columnNumber, "the quantity of arguments does not match");
        return;
    }
    for (int i = 0; i < parameterTypeList.size(); i++) {
        visit(callExpression->argumentList[i]);
        if (ErrorHandler::getStatus()) return;
        if (isVoidScalarType(callExpression->argumentList[i]->resultType)) {
            ErrorHandler::error(callExpression->lineNumber, callExpression->columnNumber, "argument can't be a void scalar");
            return;
        }
        if (!canImplicitCastOneWay(callExpression->argumentList[i]->resultType, parameterTypeList[i])) {
            ErrorHandler::error(callExpression->lineNumber, callExpression->columnNumber, "argument can't cast implicitly");
            return;
        }
    }
    callExpression->resultType = functionType->returnType->clone();
}

void ErrorCheckVisitor::visit(CastExpression *castExpression) {
    visit(castExpression->targetType);
    if (ErrorHandler::getStatus()) return;
    visit(castExpression->operand);
    if (ErrorHandler::getStatus()) return;
    castExpression->resultType = castExpression->targetType->clone();
}

void ErrorCheckVisitor::visit(CharacterLiteralExpression *characterLiteralExpression) {
    characterLiteralExpression->resultType = new ScalarType(-1, -1, BaseType::CHAR, {TypeQualifier::CONST});
}

void ErrorCheckVisitor::visit(FloatingPointLiteralExpression *floatingPointLiteralExpression) {
    floatingPointLiteralExpression->resultType = new ScalarType(-1, -1, BaseType::DOUBLE, {TypeQualifier::CONST});
}

void ErrorCheckVisitor::visit(IdentifierExpression *identifierExpression) {
    Symbol *symbol = (*symbolTableBuilder)[identifierExpression->identifier];
    if (symbol == nullptr) {
        ErrorHandler::error(identifierExpression->lineNumber, identifierExpression->columnNumber, "undefined reference to `" + identifierExpression->identifier + "`");
        return;
    }
    switch (symbol->getClass()) {
        case SymbolClass::SCALAR_SYMBOL:
            identifierExpression->isLvalue = true;
            identifierExpression->resultType = ((ScalarSymbol *) symbol)->type->clone();
            break;
        case SymbolClass::POINTER_SYMBOL:
            identifierExpression->isLvalue = true;
            identifierExpression->resultType = ((PointerSymbol *) symbol)->type->clone();
            break;
        case SymbolClass::ARRAY_SYMBOL:
            identifierExpression->isLvalue = true;
            identifierExpression->resultType = ((ArraySymbol *) symbol)->type->clone();
            break;
        case SymbolClass::FUNCTION_SYMBOL:
            identifierExpression->isLvalue = true;
            identifierExpression->resultType = ((FunctionSymbol *) symbol)->type->clone();
            break;
        case SymbolClass::STATEMENT_SYMBOL:
            identifierExpression->isLvalue = true;
            identifierExpression->resultType = ((FunctionSymbol *) symbol)->type->clone();
            return;
    }
}

void ErrorCheckVisitor::visit(IntegerLiteralExpression *integerLiteralExpression) {
    integerLiteralExpression->resultType = new ScalarType(-1, -1, BaseType::INT, {TypeQualifier::CONST});
}

void ErrorCheckVisitor::visit(StringLiteralExpression *stringLiteralExpression) {
    stringConstantPool->add(stringLiteralExpression->value);
    stringLiteralExpression->resultType = new PointerType(-1, -1, new ScalarType(-1, -1, BaseType::CHAR, {TypeQualifier::CONST}), {TypeQualifier::CONST});
}

void ErrorCheckVisitor::visit(TernaryExpression *ternaryExpression) {
    visit(ternaryExpression->leftOperand);
    if (ErrorHandler::getStatus()) return;
    visit(ternaryExpression->middleOperand);
    if (ErrorHandler::getStatus()) return;
    visit(ternaryExpression->rightOperand);
    if (ErrorHandler::getStatus()) return;
    if ((isVoidScalarType(ternaryExpression->leftOperand->resultType)) || isVoidScalarType(ternaryExpression->middleOperand->resultType) || isVoidScalarType(ternaryExpression->rightOperand->resultType)) {
        ErrorHandler::error(ternaryExpression->lineNumber, ternaryExpression->columnNumber, "void scalar can't participate in calculation");
        return;
    }
    switch (ternaryExpression->ternaryOperator) {
        case TernaryOperator::CONDITION:
            if (!(isIntegerScalarType(ternaryExpression->leftOperand->resultType) || isPointerType(ternaryExpression->leftOperand->resultType))) {
                ErrorHandler::error(ternaryExpression->lineNumber, ternaryExpression->columnNumber, "the condition of the condition ternary operator must be an integer scalar or pointer");
                return;
            }
            if (!canImplicitCastTwoWay(ternaryExpression->middleOperand->resultType, ternaryExpression->rightOperand->resultType)) {
                ErrorHandler::error(ternaryExpression->lineNumber, ternaryExpression->columnNumber, "the operand of the condition ternary operator can't cast implicitly");
                return;
            }
            ternaryExpression->resultType = implicitCastTwoWay(ternaryExpression->middleOperand->resultType, ternaryExpression->rightOperand->resultType)->clone();
            break;
    }
}

void ErrorCheckVisitor::visit(UnaryExpression *unaryExpression) {
    visit(unaryExpression->operand);
    if (ErrorHandler::getStatus()) return;
    if (isVoidScalarType(unaryExpression->operand->resultType)) {
        ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "void scalar can't participate in calculation");
        return;
    }
    switch (unaryExpression->unaryOperator) {
        case UnaryOperator::PREINCREMENT:
        case UnaryOperator::PREDECREMENT:
            if (!(isScalarType(unaryExpression->operand->resultType) || isPointerType(unaryExpression->operand->resultType))) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the increment or decrement operator must be a scalar or pointer");
                return;
            }
            if (isVoidPointerType(unaryExpression->operand->resultType)) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "void pointer can't participate in calculation");
                return;
            }
            if (haveConstTypeQualifier(unaryExpression->operand->resultType)) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "can't assign because have a const qualifier");
                return;
            }
            if (!unaryExpression->operand->isLvalue) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the left operand of the increment or decrement operator must be a lvalue");
                return;
            }
            unaryExpression->isLvalue = true;
            unaryExpression->resultType = unaryExpression->operand->resultType->clone();
            break;
        case UnaryOperator::POSTINCREMENT:
        case UnaryOperator::POSTDECREMENT:
            if (!(isScalarType(unaryExpression->operand->resultType) || isPointerType(unaryExpression->operand->resultType))) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the increment or decrement operator must be a scalar or pointer");
                return;
            }
            if (isVoidPointerType(unaryExpression->operand->resultType)) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "void pointer can't participate in calculation");
                return;
            }
            if (haveConstTypeQualifier(unaryExpression->operand->resultType)) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "can't assign because have a const qualifier");
                return;
            }
            if (!unaryExpression->operand->isLvalue) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the left operand of the increment or decrement operator must be a lvalue");
                return;
            }
            unaryExpression->isLvalue = false;
            unaryExpression->resultType = unaryExpression->operand->resultType->clone();
            break;
        case UnaryOperator::TAKE_ADDRESS:
            if (!unaryExpression->operand->isLvalue) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the left operand of the take address operator must be a lvalue");
                return;
            }
            unaryExpression->resultType = new PointerType(-1, -1, unaryExpression->operand->resultType->clone(), {TypeQualifier::CONST});
            break;
        case UnaryOperator::DEREFERENCE:
            if (isArrayType(unaryExpression->operand->resultType)) {
                if (isVoidScalarType(((ArrayType *) unaryExpression->operand->resultType)->elemType)) {
                    ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the dereference operator can't be a void array");
                    return;
                }
                unaryExpression->isLvalue = true;
                unaryExpression->resultType = ((ArrayType *) unaryExpression->operand->resultType)->elemType->clone();
            } else if (isPointerType(unaryExpression->operand->resultType)) {
                if (isVoidScalarType(((PointerType *) unaryExpression->operand->resultType)->sourceType)) {
                    ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the dereference operator can't be a void pointer");
                    return;
                }
                unaryExpression->isLvalue = true;
                unaryExpression->resultType = ((PointerType *) unaryExpression->operand->resultType)->sourceType->clone();
            } else {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the dereference operator must be a pointer or array");
                return;
            }
            break;
        case UnaryOperator::PLUS:
        case UnaryOperator::MINUS:
            if (!isIntegerScalarType(unaryExpression->operand->resultType)) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the plus or minus operator must be an integer scalar");
                return;
            }
            switch (((ScalarType *) unaryExpression->operand->resultType)->baseType) {
                case BaseType::VOID:
                    assert(false); // 前面检查过了
                case BaseType::CHAR:
                case BaseType::SHORT:
                case BaseType::INT:
                case BaseType::LONG_INT:
                case BaseType::LONG_LONG_INT:
                case BaseType::FLOAT:
                case BaseType::DOUBLE:
                    unaryExpression->resultType = unaryExpression->operand->resultType->clone();
                    break;
                case BaseType::UNSIGNED_CHAR:
                    unaryExpression->resultType = new ScalarType(-1, -1, BaseType::CHAR, {TypeQualifier::CONST});
                    break;
                case BaseType::UNSIGNED_SHORT:
                    unaryExpression->resultType = new ScalarType(-1, -1, BaseType::SHORT, {TypeQualifier::CONST});
                    break;
                case BaseType::UNSIGNED_INT:
                case BaseType::UNSIGNED_LONG_INT:
                    unaryExpression->resultType = new ScalarType(-1, -1, BaseType::INT, {TypeQualifier::CONST});
                    break;
                case BaseType::UNSIGNED_LONG_LONG_INT:
                    unaryExpression->resultType = new ScalarType(-1, -1, BaseType::LONG_LONG_INT, {TypeQualifier::CONST});
                    break;
            }
            break;
        case UnaryOperator::BITWISE_NOT:
            if (!isIntegerScalarType(unaryExpression->operand->resultType)) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the bitwise not operator must be an integer scalar");
                return;
            }
            unaryExpression->resultType = unaryExpression->operand->resultType->clone();
            break;
        case UnaryOperator::LOGICAL_NOT:
            if (!(isIntegerScalarType(unaryExpression->operand->resultType) || isPointerType(unaryExpression->operand->resultType))) {
                ErrorHandler::error(unaryExpression->lineNumber, unaryExpression->columnNumber, "the operand of the logical not operator must be an integer scalar or pointer");
                return;
            }
            unaryExpression->resultType = new ScalarType(-1, -1, BaseType::LONG_LONG_INT, {TypeQualifier::CONST});
            break;
        case UnaryOperator::SIZEOF:
            unaryExpression->resultType = new ScalarType(-1, -1, BaseType::UNSIGNED_LONG_LONG_INT, {TypeQualifier::CONST});
            break;
    }
}

void ErrorCheckVisitor::visit(ArrayType *arrayType) {
    visit(arrayType->elemType);
    if (ErrorHandler::getStatus()) return;
}

void ErrorCheckVisitor::visit(FunctionType *functionType) {
    visit(functionType->returnType);
    if (ErrorHandler::getStatus()) return;
    for (auto parameterType : functionType->parameterTypeList) {
        visit(parameterType);
        if (ErrorHandler::getStatus()) return;
    }
}

void ErrorCheckVisitor::visit(PointerType *pointerType) {
    visit(pointerType->sourceType);
    if (ErrorHandler::getStatus()) return;
}

void ErrorCheckVisitor::visit(ScalarType *scalarType) {
}

void ErrorCheckVisitor::visit(FunctionDeclaration *functionDeclaration) {
    visit(functionDeclaration->functionType);
    if (ErrorHandler::getStatus()) return;
    undefinedFunctionSet.insert(functionDeclaration->identifier);
    if ((*symbolTableBuilder)[functionDeclaration->identifier] == nullptr) {
        symbolTableBuilder->insertSymbol(new FunctionSymbol(functionDeclaration->identifier, (FunctionType *) functionDeclaration->functionType->clone()));
    }
}

void ErrorCheckVisitor::visit(FunctionDefinition *functionDefinition) {
    undefinedFunctionSet.erase(functionDefinition->identifier);
    if (functionDefinition->identifier == "main") {
        haveEntryFunction = true;
    }
    if ((*symbolTableBuilder)[functionDefinition->identifier] == nullptr) {
        symbolTableBuilder->insertSymbol(new FunctionSymbol(functionDefinition->identifier, (FunctionType *) functionDefinition->functionType->clone()));
    }
    symbolTableBuilder->createScope(functionDefinition->identifier);
    for (auto parameterDeclaration : functionDefinition->parameterDeclarationList) {
        if (parameterDeclaration->getClass() == DeclarationClass::FUNCTION_DECLARATION || parameterDeclaration->getClass() == DeclarationClass::FUNCTION_DEFINITION) {
            ErrorHandler::error(functionDefinition->lineNumber, functionDefinition->columnNumber, "invalid definition of `" + functionDefinition->identifier + "` because parameter can't be a function");
            return;
        }
        visit(parameterDeclaration);
        if (ErrorHandler::getStatus()) return;
    }
    currentFunctionType = (FunctionType *) functionDefinition->functionType;
    visit(functionDefinition->body);
    if (ErrorHandler::getStatus()) return;
    currentFunctionType = nullptr;
    symbolTableBuilder->exitScope();
}

void ErrorCheckVisitor::visit(VariableDeclaration *variableDeclaration) {
    visit(variableDeclaration->variableType);
    if (ErrorHandler::getStatus()) return;
    for (auto initialValue : variableDeclaration->initialValueList) {
        visit(initialValue);
        if (ErrorHandler::getStatus()) return;
    }
    if ((*symbolTableBuilder)[variableDeclaration->identifier] != nullptr) {
        ErrorHandler::error(variableDeclaration->lineNumber, variableDeclaration->columnNumber, "multiple definition of `" + variableDeclaration->identifier + "`");
        return;
    }
    switch (variableDeclaration->variableType->getClass()) {
        case TypeClass::ARRAY_TYPE: {
            if (isVoidScalarType(((ArrayType *) variableDeclaration->variableType)->elemType)) {
                ErrorHandler::error(variableDeclaration->lineNumber, variableDeclaration->columnNumber, "invalid definition of `" + variableDeclaration->identifier + "` because the elem of the array can't be a void scalar");
                return;
            }
            if (((ArrayType *) variableDeclaration->variableType)->size < 1) {
                ErrorHandler::error(variableDeclaration->lineNumber, variableDeclaration->columnNumber, "invalid definition of `" + variableDeclaration->identifier + "` because the size of the array is invalid");
                return;
            }
            if (!variableDeclaration->initialValueList.empty() && ((ArrayType *) variableDeclaration->variableType)->size != variableDeclaration->initialValueList.size()) {
                ErrorHandler::error(variableDeclaration->lineNumber, variableDeclaration->columnNumber, "invalid definition of `" + variableDeclaration->identifier + "` because the size of the array is not equal to the size of the initial value list");
                return;
            }
            symbolTableBuilder->insertSymbol(new ArraySymbol(variableDeclaration->identifier, (ArrayType *) variableDeclaration->variableType->clone()));
            break;
        }
        case TypeClass::SCALAR_TYPE: {
            if (isVoidScalarType(variableDeclaration->variableType)) {
                ErrorHandler::error(variableDeclaration->lineNumber, variableDeclaration->columnNumber, "invalid definition of `" + variableDeclaration->identifier + "` because it is a void scalar");
                return;
            }
            if (variableDeclaration->initialValueList.size() > 1) {
                ErrorHandler::error(variableDeclaration->lineNumber, variableDeclaration->columnNumber, "invalid definition of `" + variableDeclaration->identifier + "` because it has multiple initial values");
                return;
            }
            symbolTableBuilder->insertSymbol(new ScalarSymbol(variableDeclaration->identifier, (ScalarType *) variableDeclaration->variableType->clone()));
            break;
        }
        case TypeClass::FUNCTION_TYPE:
            assert(false);
        case TypeClass::POINTER_TYPE: {
            if (variableDeclaration->initialValueList.size() > 1 && !isCharPointerType(variableDeclaration->variableType)) {
                ErrorHandler::error(variableDeclaration->lineNumber, variableDeclaration->columnNumber, "invalid definition of `" + variableDeclaration->identifier + "` because it has multiple initial values");
                return;
            }
            symbolTableBuilder->insertSymbol(new PointerSymbol(variableDeclaration->identifier, (PointerType *) variableDeclaration->variableType->clone()));
            break;
        }
    }
}

void ErrorCheckVisitor::visit(BreakStatement *breakStatement) {
    if (levelCanBreak <= 0) {
        ErrorHandler::error(breakStatement->lineNumber, breakStatement->columnNumber, "break statement must be in a loop or switch statement");
        return;
    }
}

void ErrorCheckVisitor::visit(CaseStatement *caseStatement) {
    if (levelSwitch <= 0) {
        ErrorHandler::error(caseStatement->lineNumber, caseStatement->columnNumber, "case statement must be in a switch statement");
        return;
    }
    if (caseStatement->statement != nullptr) {
        visit(caseStatement->statement);
        if (ErrorHandler::getStatus()) return;
    }
}

void ErrorCheckVisitor::visit(CompoundStatement *compoundStatement) {
    symbolTableBuilder->createScope("");
    for (Statement *statement : compoundStatement->statementList) {
        visit(statement);
        if (ErrorHandler::getStatus()) return;
    }
    symbolTableBuilder->exitScope();
}

void ErrorCheckVisitor::visit(ContinueStatement *continueStatement) {
    if (levelCanContinue <= 0) {
        ErrorHandler::error(continueStatement->lineNumber, continueStatement->columnNumber, "continue statement must be in a loop statement");
        return;
    }
}

void ErrorCheckVisitor::visit(DeclarationStatement *declarationStatement) {
    for (Declaration *declaration : declarationStatement->declarationList) {
        visit(declaration);
        if (ErrorHandler::getStatus()) return;
    }
}

void ErrorCheckVisitor::visit(DefaultStatement *defaultStatement) {
    if (levelSwitch <= 0) {
        ErrorHandler::error(defaultStatement->lineNumber, defaultStatement->columnNumber, "default statement must be in a switch statement");
        return;
    }
    if (defaultStatement->statement != nullptr) {
        visit(defaultStatement->statement);
        if (ErrorHandler::getStatus()) return;
    }
}

void ErrorCheckVisitor::visit(DoWhileStatement *doWhileStatement) {
    levelCanBreak++;
    levelCanContinue++;
    visit(doWhileStatement->body);
    if (ErrorHandler::getStatus()) return;
    levelCanBreak--;
    levelCanContinue--;
    visit(doWhileStatement->condition);
    if (ErrorHandler::getStatus()) return;
    if (!(isIntegerScalarType(doWhileStatement->condition->resultType) || isPointerType(doWhileStatement->condition->resultType))) {
        ErrorHandler::error(doWhileStatement->lineNumber, doWhileStatement->columnNumber, "the condition of the do-while statement must be an integer scalar or pointer");
        return;
    }
}

void ErrorCheckVisitor::visit(ExpressionStatement *expressionStatement) {
    if (expressionStatement->expression != nullptr) {
        visit(expressionStatement->expression);
        if (ErrorHandler::getStatus()) return;
    }
}

void ErrorCheckVisitor::visit(ForStatement *forStatement) {
    symbolTableBuilder->createScope("");
    for (auto initDeclaration : forStatement->declarationList) {
        visit(initDeclaration);
        if (ErrorHandler::getStatus()) return;
    }
    if (forStatement->init != nullptr) {
        visit(forStatement->init);
        if (ErrorHandler::getStatus()) return;
    }
    if (forStatement->condition != nullptr) {
        visit(forStatement->condition);
        if (ErrorHandler::getStatus()) return;
        if (!(isIntegerScalarType(forStatement->condition->resultType) || isPointerType(forStatement->condition->resultType))) {
            ErrorHandler::error(forStatement->lineNumber, forStatement->columnNumber, "the condition of the for statement must be an integer scalar or pointer");
            return;
        }
    }
    if (forStatement->update != nullptr) {
        visit(forStatement->update);
        if (ErrorHandler::getStatus()) return;
    }
    levelCanBreak++;
    levelCanContinue++;
    visit(forStatement->body);
    if (ErrorHandler::getStatus()) return;
    levelCanBreak--;
    levelCanContinue--;
    symbolTableBuilder->exitScope();
}

void ErrorCheckVisitor::visit(GotoStatement *gotoStatement) {
    if ((*symbolTableBuilder)[gotoStatement->identifier] == nullptr) {
        ErrorHandler::error(gotoStatement->lineNumber, gotoStatement->columnNumber, "undefined reference to `" + gotoStatement->identifier + "`");
        return;
    }
    Symbol *symbol = (*symbolTableBuilder)[gotoStatement->identifier];
    if (symbol->getClass() != SymbolClass::STATEMENT_SYMBOL) {
        ErrorHandler::error(gotoStatement->lineNumber, gotoStatement->columnNumber, "the identifier of the goto statement must be a statement identifier");
        return;
    }
}

void ErrorCheckVisitor::visit(IfStatement *ifStatement) {
    visit(ifStatement->condition);
    if (ErrorHandler::getStatus()) return;
    if (!(isIntegerScalarType(ifStatement->condition->resultType) || isPointerType(ifStatement->condition->resultType))) {
        ErrorHandler::error(ifStatement->lineNumber, ifStatement->columnNumber, "the condition of the if statement must be an integer scalar or pointer");
        return;
    }
    visit(ifStatement->trueBody);
    if (ErrorHandler::getStatus()) return;
    if (ifStatement->falseBody != nullptr) {
        visit(ifStatement->falseBody);
        if (ErrorHandler::getStatus()) return;
    }
}

void ErrorCheckVisitor::visit(LabelStatement *labelStatement) {
    if ((*symbolTableBuilder)[labelStatement->identifier] != nullptr) {
        ErrorHandler::error(labelStatement->lineNumber, labelStatement->columnNumber, "multiple definition of `" + labelStatement->identifier + "`");
        return;
    }
    symbolTableBuilder->insertSymbol(new StatementSymbol(labelStatement->identifier));
    visit(labelStatement->statement);
    if (ErrorHandler::getStatus()) return;
}

void ErrorCheckVisitor::visit(ReturnStatement *returnStatement) {
    if (currentFunctionType == nullptr) {
        ErrorHandler::error(returnStatement->lineNumber, returnStatement->columnNumber, "return statement must be in a function definition");
        return;
    }
    if (returnStatement->value == nullptr) {
        if (!isVoidScalarType(currentFunctionType->returnType)) {
            ErrorHandler::error(returnStatement->lineNumber, returnStatement->columnNumber, "the return value type of the return statement does not match");
            return;
        }
    } else {
        visit(returnStatement->value);
        if (ErrorHandler::getStatus()) return;
        if (isVoidScalarType(currentFunctionType->returnType)) {
            ErrorHandler::error(returnStatement->lineNumber, returnStatement->columnNumber, "the return value type of the return statement does not match");
            return;
        }
        if (!canImplicitCastOneWay(returnStatement->value->resultType, currentFunctionType->returnType)) {
            ErrorHandler::error(returnStatement->lineNumber, returnStatement->columnNumber, "the return value type of the return statement does not match");
            return;
        }
    }

}

void ErrorCheckVisitor::visit(SwitchStatement *switchStatement) {
    visit(switchStatement->expression);
    if (ErrorHandler::getStatus()) return;
    if (!isIntegerScalarType(switchStatement->expression->resultType)) {
        ErrorHandler::error(switchStatement->lineNumber, switchStatement->columnNumber, "the expression of the switch statement must be a integer scalar");
        return;
    }
    if (switchStatement->body->getClass() != StatementClass::COMPOUND_STATEMENT) {
        ErrorHandler::error(switchStatement->lineNumber, switchStatement->columnNumber, "the body of the switch statement must be a compound statement consisting of case statement");
        return;
    }
    bool haveDefaultStatement = false;
    std::vector<std::int64_t> caseValueList;
    for (auto statement : ((CompoundStatement *) switchStatement->body)->statementList) {
        if (statement->getClass() == StatementClass::CASE_STATEMENT) {
            if (std::find(caseValueList.begin(), caseValueList.end(), ((CaseStatement *) statement)->value) != caseValueList.end()) {
                ErrorHandler::error(switchStatement->lineNumber, switchStatement->columnNumber, "the body of the switch statement have repeated case statement");
                return;
            }
            caseValueList.push_back(((CaseStatement *) statement)->value);
        } else if (statement->getClass() == StatementClass::DEFAULT_STATEMENT) {
            if (haveDefaultStatement) {
                ErrorHandler::error(switchStatement->lineNumber, switchStatement->columnNumber, "the body of the switch statement have repeated case statement");
                return;
            }
            haveDefaultStatement = true;
        } else {
            ErrorHandler::error(switchStatement->lineNumber, switchStatement->columnNumber, "the body of the switch statement must be a compound statement consisting of case statement");
            return;
        }
    }
    levelSwitch++;
    levelCanBreak++;
    visit(switchStatement->body);
    if (ErrorHandler::getStatus()) return;
    levelCanBreak--;
    levelSwitch--;
}

void ErrorCheckVisitor::visit(WhileStatement *whileStatement) {
    visit(whileStatement->condition);
    if (ErrorHandler::getStatus()) return;
    levelCanBreak++;
    levelCanContinue++;
    visit(whileStatement->body);
    if (ErrorHandler::getStatus()) return;
    levelCanBreak--;
    levelCanContinue--;
    if (!(isIntegerScalarType(whileStatement->condition->resultType) || isPointerType(whileStatement->condition->resultType))) {
        ErrorHandler::error(whileStatement->lineNumber, whileStatement->columnNumber, "the condition of the while statement must be an integer scalar or pointer");
        return;
    }
}

void ErrorCheckVisitor::visit(TranslationUnit *translationUnit) {
    BuiltInFunctionLibrary::insertSymbol(symbolTableBuilder);
    for (auto declaration : translationUnit->declarationList) {
        visit(declaration);
        if (ErrorHandler::getStatus()) return;
    }
    if (!haveEntryFunction) {
        ErrorHandler::error(translationUnit->lineNumber, translationUnit->columnNumber, "entry function not found");
        return;
    }
    if (!undefinedFunctionSet.empty()) {
        ErrorHandler::error(translationUnit->lineNumber, translationUnit->columnNumber, "existence of declared but undefined functions");
        return;
    }
}

void ErrorCheckVisitor::checkError(TranslationUnit *translationUnit, SymbolTable *&symbolTable, StringConstantPool *&stringConstantPool) {
    auto *errorCheckVisitor = new ErrorCheckVisitor();
    translationUnit->accept(errorCheckVisitor);
    symbolTable = errorCheckVisitor->symbolTableBuilder->build();
    stringConstantPool = errorCheckVisitor->stringConstantPool;
    delete errorCheckVisitor;
}

