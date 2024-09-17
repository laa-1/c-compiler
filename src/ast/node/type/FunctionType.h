#pragma once

#include "../Type.h"
#include <vector>

class FunctionType : public Type {
public:
    Type *returnType;
    std::vector<Type *> parameterTypeList;

    FunctionType(int lineNumber, int columnNumber, Type *returnType, const std::vector<Type *> &parameterTypeList) : Type(lineNumber, columnNumber), returnType(returnType), parameterTypeList(parameterTypeList) {}

    ~FunctionType() override {
        delete returnType;
        for (auto parameterType : parameterTypeList) {
            delete parameterType;
        }
    }

    TypeClass getClass() override {
        return TypeClass::FUNCTION_TYPE;
    }

    Type *clone() override {
        std::vector<Type *> newParameterTypeList;
        newParameterTypeList.reserve(parameterTypeList.size());
        for (auto parameterType : parameterTypeList) {
            newParameterTypeList.push_back(parameterType->clone());
        }
        return new FunctionType(lineNumber, columnNumber, returnType->clone(), newParameterTypeList);
    }
};
