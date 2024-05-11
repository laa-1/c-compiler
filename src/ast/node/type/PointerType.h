#pragma once

#include "../Type.h"
#include <vector>

class PointerType : public Type {
public:
    Type *sourceType;
    std::vector<TypeQualifier> typeQualifierList;

    PointerType(Type *sourceType, const std::vector<TypeQualifier> &typeQualifierList);
    ~PointerType() override;
    TypeClass getClass() override;
    Type *clone() override;
};
