#include "PointerType.h"

#include <iostream>

PointerType::PointerType(Type *sourceType, const std::vector<TypeQualifier> &typeQualifierList) : sourceType(sourceType), typeQualifierList(typeQualifierList) {}

PointerType::~PointerType() {
    delete sourceType;
}

TypeClass PointerType::getClass() {
    return TypeClass::POINTER_TYPE;
}

Type *PointerType::clone() {
    return new PointerType(sourceType->clone(), typeQualifierList);
}
