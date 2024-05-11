#pragma once

#include <vector>
#include "../Type.h"
#include "../Expression.h"

class ArrayType : public Type {
public:
    Type *elemType;
    int size;

    ArrayType(Type *elemType, int size);
    ~ArrayType() override;
    TypeClass getClass() override;
    Type *clone() override;
};
