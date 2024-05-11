#pragma once

#include "../Type.h"
#include <vector>

class FunctionType : public Type {
public:
    Type *returnType;
    std::vector<Type *> parameterTypeList;

    FunctionType(Type *returnType, const std::vector<Type *> &parameterTypeList);
    ~FunctionType() override;
    TypeClass getClass() override;
    Type *clone() override;
};
