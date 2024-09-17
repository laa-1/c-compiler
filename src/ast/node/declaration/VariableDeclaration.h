#pragma once

#include <utility>
#include <vector>
#include "../Type.h"
#include "../Declaration.h"
#include "../Expression.h"

class VariableDeclaration : public Declaration {
public:
    std::vector<StorageSpecifier> storageSpecifierList;
    Type *variableType;
    std::string identifier;
    std::vector<Expression *> initialValueList;

    VariableDeclaration(int lineNumber, int columnNumber, const std::vector<StorageSpecifier> &storageSpecifierList, Type *variableType, std::string identifier, const std::vector<Expression *> &initialValueList) : Declaration(lineNumber, columnNumber), storageSpecifierList(storageSpecifierList), variableType(variableType), identifier(std::move(identifier)), initialValueList(initialValueList) {}

    ~VariableDeclaration() override {
        delete variableType;
        for (auto initialValue : initialValueList) {
            delete initialValue;
        }
    }

    DeclarationClass getClass() override {
        return DeclarationClass::VARIABLE_DECLARATION;
    }
};
