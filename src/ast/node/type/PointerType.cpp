#include "PointerType.h"

#include <iostream>

PointerType::PointerType(int lineNumber, int columnNumber, Type *sourceType, const std::vector<TypeQualifier> &typeQualifierList) : Type(lineNumber, columnNumber), sourceType(sourceType), typeQualifierList(typeQualifierList) {}

PointerType::~PointerType() {
    delete sourceType;
}

TypeClass PointerType::getClass() {
    return TypeClass::POINTER_TYPE;
}

Type *PointerType::clone() {
    return new PointerType(lineNumber, columnNumber, sourceType->clone(), typeQualifierList);
}

