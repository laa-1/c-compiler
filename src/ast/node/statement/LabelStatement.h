#pragma once

#include "../Statement.h"

class LabelStatement : public Statement {
public:
    std::string identifier;
    Statement *statement;

    LabelStatement(int lineNumber, int columnNumber, std::string identifier, Statement *statement) : Statement(lineNumber, columnNumber), identifier(std::move(identifier)), statement(statement) {}

    ~LabelStatement() override {
        delete statement;
    }

    StatementClass getClass() override {
        return StatementClass::LABEL_STATEMENT;
    }
};
