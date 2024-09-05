#include "DeclarationStatement.h"

#include <iostream>

DeclarationStatement::DeclarationStatement(int lineNumber, int columnNumber, const std::vector<Declaration *> &declarationList) : Statement(lineNumber, columnNumber), declarationList(declarationList) {}

DeclarationStatement::~DeclarationStatement() {
    for (auto declaration : declarationList) {
        delete declaration;
    }
}

StatementClass DeclarationStatement::getClass() {
    return StatementClass::DECLARATION_STATEMENT;
}