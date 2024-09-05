#include "FunctionType.h"
#include <iostream>

FunctionType::FunctionType(int lineNumber, int columnNumber, Type *returnType, const std::vector<Type *> &parameterTypeList) : Type(lineNumber, columnNumber), returnType(returnType), parameterTypeList(parameterTypeList) {}

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
    return new FunctionType(lineNumber, columnNumber, returnType->clone(), newParameterTypeList);
}



