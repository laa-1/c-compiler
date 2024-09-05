#include "ArrayType.h"
#include <iostream>

ArrayType::ArrayType(int lineNumber, int columnNumber, Type *elemType, int size) : Type(lineNumber, columnNumber), elemType(elemType), size(size) {}

ArrayType::~ArrayType() {
    delete elemType;
}

TypeClass ArrayType::getClass() {
    return TypeClass::ARRAY_TYPE;
}

Type *ArrayType::clone() {
    return new ArrayType(lineNumber, columnNumber, elemType->clone(), size);
}



