#include "FunctionDefinition.h"
#include <iostream>
#include <utility>

FunctionDefinition::FunctionDefinition(const std::vector<FunctionSpecifier> &functionSpecifierList, Type *functionType, std::string identifier, const std::vector<Declaration *> &parameterDeclarationList, Statement *body) : functionSpecifierList(functionSpecifierList), functionType(functionType), identifier(std::move(identifier)), parameterDeclarationList(parameterDeclarationList), body(body) {}

FunctionDefinition::~FunctionDefinition() {
    delete functionType;
    delete body;
    for (auto parameterDeclaration : parameterDeclarationList) {
        delete parameterDeclaration;
    }
}

DeclarationClass FunctionDefinition::getClass() {
    return DeclarationClass::FUNCTION_DEFINITION;
}



