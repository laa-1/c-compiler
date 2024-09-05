#pragma once

#include "../Type.h"
#include <vector>

class FunctionType : public Type {
public:
    Type *returnType;
    std::vector<Type *> parameterTypeList;

    FunctionType(int lineNumber, int columnNumber, Type *returnType, const std::vector<Type *> &parameterTypeList);
    ~FunctionType() override;
    TypeClass getClass() override;
    Type *clone() override;
};
