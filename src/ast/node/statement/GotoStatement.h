#pragma once

#include <utility>

#include "../Statement.h"

class GotoStatement : public Statement {
public:
    std::string identifier;

    GotoStatement(int lineNumber, int columnNumber, std::string identifier) : Statement(lineNumber, columnNumber), identifier(std::move(identifier)) {}

    ~GotoStatement() override = default;

    StatementClass getClass() override {
        return StatementClass::GOTO_STATEMENT;
    }
};
