#pragma once

#include "../Statement.h"
#include <vector>

class CompoundStatement : public Statement {
public:
    std::vector<Statement *> statementList;

    CompoundStatement(int lineNumber, int columnNumber, const std::vector<Statement *> &statementList) : Statement(lineNumber, columnNumber), statementList(statementList) {}

    ~CompoundStatement() override {
        for (auto statement : statementList) {
            delete statement;
        }
    }

    StatementClass getClass() override {
        return StatementClass::COMPOUND_STATEMENT;
    }
};
