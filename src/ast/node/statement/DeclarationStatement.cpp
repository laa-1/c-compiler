#include "DeclarationStatement.h"

#include <iostream>

DeclarationStatement::DeclarationStatement(const std::vector<Declaration *> &declarationList) : declarationList(declarationList) {}

DeclarationStatement::~DeclarationStatement() {
    for (auto declaration : declarationList) {
        delete declaration;
    }
}

StatementClass DeclarationStatement::getClass() {
    return StatementClass::DECLARATION_STATEMENT;
}
