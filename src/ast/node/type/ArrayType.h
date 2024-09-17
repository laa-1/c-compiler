#pragma once

#include <vector>
#include "../Type.h"
#include "../Expression.h"

class ArrayType : public Type {
public:
    Type *elemType;
    int size;

    ArrayType(int lineNumber, int columnNumber, Type *elemType, int size) : Type(lineNumber, columnNumber), elemType(elemType), size(size) {}

    ~ArrayType() override {
        delete elemType;
    }

    TypeClass getClass() override {
        return TypeClass::ARRAY_TYPE;
    }

    Type *clone() override {
        return new ArrayType(lineNumber, columnNumber, elemType->clone(), size);
    }
};
