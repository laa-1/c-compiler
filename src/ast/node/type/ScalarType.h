#pragma once

#include "../Type.h"
#include <vector>

class ScalarType : public Type {
public:
    BaseType baseType;
    std::vector<TypeQualifier> typeQualifierList;

    ScalarType(int lineNumber, int columnNumber, BaseType baseType, const std::vector<TypeQualifier> &typeQualifierList) : Type(lineNumber, columnNumber), baseType(baseType), typeQualifierList(typeQualifierList) {}

    ~ScalarType() override = default;

    TypeClass getClass() override {
        return TypeClass::SCALAR_TYPE;
    }

    Type *clone() override {
        return new ScalarType(lineNumber, columnNumber, baseType, typeQualifierList);
    }
};
