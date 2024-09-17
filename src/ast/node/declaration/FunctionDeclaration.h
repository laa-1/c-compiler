#pragma once

#include <vector>
#include "../Type.h"
#include "../Declaration.h"
#include "../Expression.h"
#include "../Statement.h"

class FunctionDeclaration : public Declaration {
public:
    std::vector<FunctionSpecifier> functionSpecifierList;
    Type *functionType;
    std::string identifier;

    FunctionDeclaration(int lineNumber, int columnNumber, const std::vector<FunctionSpecifier> &functionSpecifierList, Type *functionType, std::string identifier) : Declaration(lineNumber, columnNumber), functionSpecifierList(functionSpecifierList), functionType(functionType), identifier(std::move(identifier)) {}

    ~FunctionDeclaration() override {
        delete functionType;
    }

    DeclarationClass getClass() override {
        return DeclarationClass::FUNCTION_DECLARATION;
    }
};
