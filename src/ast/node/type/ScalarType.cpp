#include "ScalarType.h"
#include <iostream>

ScalarType::ScalarType(int lineNumber, int columnNumber, BaseType baseType, const std::vector<TypeQualifier> &typeQualifierList) : Type(lineNumber, columnNumber), baseType(baseType), typeQualifierList(typeQualifierList) {}

TypeClass ScalarType::getClass() {
    return TypeClass::SCALAR_TYPE;
}

Type *ScalarType::clone() {
    return new ScalarType(lineNumber, columnNumber, baseType, typeQualifierList);
}





