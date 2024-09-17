#pragma once

#include <vector>
#include "../Statement.h"
#include "../Declaration.h"

class DeclarationStatement : public Statement {
public:
    std::vector<Declaration *> declarationList;

    DeclarationStatement(int lineNumber, int columnNumber, const std::vector<Declaration *> &declarationList) : Statement(lineNumber, columnNumber), declarationList(declarationList) {}

    ~DeclarationStatement() override {
        for (auto declaration : declarationList) {
            delete declaration;
        }
    }

    StatementClass getClass() override {
        return StatementClass::DECLARATION_STATEMENT;
    }
};
