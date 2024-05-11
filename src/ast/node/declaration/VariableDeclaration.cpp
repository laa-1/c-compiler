#include "VariableDeclaration.h"

#include <utility>
#include <iostream>

VariableDeclaration::VariableDeclaration(const std::vector<StorageSpecifier> &storageSpecifierList, Type *variableType, std::string identifier, const std::vector<Expression *> &initialValueList) : storageSpecifierList(storageSpecifierList), variableType(variableType), identifier(std::move(identifier)), initialValueList(initialValueList) {}

VariableDeclaration::~VariableDeclaration() {
    delete variableType;
    for (auto initialValue : initialValueList) {
        delete initialValue;
    }
}

DeclarationClass VariableDeclaration::getClass() {
    return DeclarationClass::VARIABLE_DECLARATION;
}
