#include "FunctionType.h"
#include <iostream>

FunctionType::FunctionType(Type *returnType, const std::vector<Type *> &parameterTypeList) : returnType(returnType), parameterTypeList(parameterTypeList) {}

FunctionType::~FunctionType() {
    delete returnType;
    for (auto parameterType : parameterTypeList) {
        delete parameterType;
    }
}

TypeClass FunctionType::getClass() {
    return TypeClass::FUNCTION_TYPE;
}

Type *FunctionType::clone() {
    std::vector<Type *> newParameterTypeList;
    newParameterTypeList.reserve(parameterTypeList.size());
    for (auto parameterType : parameterTypeList) {
        newParameterTypeList.push_back(parameterType->clone());
    }
    return new FunctionType(returnType->clone(), newParameterTypeList);
}


