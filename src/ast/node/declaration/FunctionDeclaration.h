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

    FunctionDeclaration(const std::vector<FunctionSpecifier> &functionSpecifierList, Type *functionType, std::string identifier);
    ~FunctionDeclaration() override;
    DeclarationClass getClass() override;
};
