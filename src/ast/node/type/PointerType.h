#pragma once

#include "../Type.h"
#include <vector>

class PointerType : public Type {
public:
    Type *sourceType;
    std::vector<TypeQualifier> typeQualifierList;

    PointerType(int lineNumber, int columnNumber, Type *sourceType, const std::vector<TypeQualifier> &typeQualifierList) : Type(lineNumber, columnNumber), sourceType(sourceType), typeQualifierList(typeQualifierList) {}

    ~PointerType() override {
        delete sourceType;
    }

    TypeClass getClass() override {
        return TypeClass::POINTER_TYPE;
    }

    Type *clone() override {
        return new PointerType(lineNumber, columnNumber, sourceType->clone(), typeQualifierList);
    }
};
