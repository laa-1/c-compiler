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

    FunctionDefinition(const std::vector<FunctionSpecifier> &functionSpecifierList, Type *functionType, std::string identifier, const std::vector<Declaration *> &parameterDeclarationList, Statement *body);
    ~FunctionDefinition() override;
    DeclarationClass getClass() override;
};
