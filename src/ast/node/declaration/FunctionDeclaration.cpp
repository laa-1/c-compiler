#include "FunctionDeclaration.h"

#include <iostream>
#include <utility>

FunctionDeclaration::FunctionDeclaration(int lineNumber, int columnNumber, const std::vector<FunctionSpecifier> &functionSpecifierList, Type *functionType, std::string identifier) : Declaration(lineNumber, columnNumber), functionSpecifierList(functionSpecifierList), functionType(functionType), identifier(std::move(identifier)) {}

FunctionDeclaration::~FunctionDeclaration() {
    delete functionType;
}

DeclarationClass FunctionDeclaration::getClass() {
    return DeclarationClass::FUNCTION_DECLARATION;
}



