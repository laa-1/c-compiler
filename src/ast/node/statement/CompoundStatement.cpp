#include "CompoundStatement.h"

#include <iostream>

CompoundStatement::CompoundStatement(int lineNumber, int columnNumber, const std::vector<Statement *> &statementList) : Statement(lineNumber, columnNumber), statementList(statementList) {}

CompoundStatement::~CompoundStatement() {
    for (auto statement : statementList) {
        delete statement;
    }
}

StatementClass CompoundStatement::getClass() {
    return StatementClass::COMPOUND_STATEMENT;
}