#pragma once

#include "../Type.h"
#include <vector>

class ScalarType : public Type {
public:
    BaseType baseType;
    std::vector<TypeQualifier> typeQualifierList;

    ScalarType(int lineNumber, int columnNumber, BaseType baseType, const std::vector<TypeQualifier> &typeQualifierList);
    ~ScalarType() override = default;
    TypeClass getClass() override;
    Type *clone() override;
};
