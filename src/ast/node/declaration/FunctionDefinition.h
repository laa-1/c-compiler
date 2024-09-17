#pragma once

#include <vector>
#include "../Type.h"
#include "../Declaration.h"
#include "../Expression.h"
#include "../Statement.h"

class FunctionDefinition : public Declaration {
public:
    std::vector<FunctionSpecifier> functionSpecifierList;
    Type *functionType;
    std::string identifier;
    std::vector<Declaration *> parameterDeclarationList;
    Statement *body;

    FunctionDefinition(int lineNumber, int columnNumber, const std::vector<FunctionSpecifier> &functionSpecifierList, Type *functionType, std::string identifier, const std::vector<Declaration *> &parameterDeclarationList, Statement *body) : Declaration(lineNumber, columnNumber), functionSpecifierList(functionSpecifierList), functionType(functionType), identifier(std::move(identifier)), parameterDeclarationList(parameterDeclarationList), body(body) {}

    ~FunctionDefinition() override {
        delete functionType;
        delete body;
        for (auto parameterDeclaration : parameterDeclarationList) {
            delete parameterDeclaration;
        }
    }

    DeclarationClass getClass() override {
        return DeclarationClass::FUNCTION_DEFINITION;
    }
};
