#include "VariableDeclaration.h"

#include <utility>
#include <iostream>

VariableDeclaration::VariableDeclaration(int lineNumber, int columnNumber, const std::vector<StorageSpecifier> &storageSpecifierList, Type *variableType, const std::string &identifier, const std::vector<Expression *> &initialValueList) : Declaration(lineNumber, columnNumber), storageSpecifierList(storageSpecifierList), variableType(variableType), identifier(identifier), initialValueList(initialValueList) {}

VariableDeclaration::~VariableDeclaration() {
    delete variableType;
    for (auto initialValue : initialValueList) {
        delete initialValue;
    }
}

DeclarationClass VariableDeclaration::getClass() {
    return DeclarationClass::VARIABLE_DECLARATION;
}

