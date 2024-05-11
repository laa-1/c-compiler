#include "ScalarType.h"
#include <iostream>

ScalarType::ScalarType(BaseType baseType, const std::vector<TypeQualifier> &typeQualifierList) : baseType(baseType), typeQualifierList(typeQualifierList) {}

TypeClass ScalarType::getClass() {
    return TypeClass::SCALAR_TYPE;
}

Type *ScalarType::clone() {
    return new ScalarType(baseType, typeQualifierList);
}




