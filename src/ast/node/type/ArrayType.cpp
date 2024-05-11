#include "ArrayType.h"
#include <iostream>

ArrayType::ArrayType(Type *elemType, int size) : elemType(elemType), size(size) {}

ArrayType::~ArrayType() {
    delete elemType;
}

TypeClass ArrayType::getClass() {
    return TypeClass::ARRAY_TYPE;
}

Type *ArrayType::clone() {
    return new ArrayType(elemType->clone(), size);
}


