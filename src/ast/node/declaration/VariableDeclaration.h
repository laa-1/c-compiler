#pragma once

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

    VariableDeclaration(int lineNumber, int columnNumber, const std::vector<StorageSpecifier> &storageSpecifierList, Type *variableType, const std::string &identifier, const std::vector<Expression *> &initialValueList);
    ~VariableDeclaration() override;
    DeclarationClass getClass() override;
};
