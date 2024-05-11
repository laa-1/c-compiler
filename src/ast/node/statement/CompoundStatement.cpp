#include "CompoundStatement.h"

#include <iostream>

CompoundStatement::CompoundStatement(const std::vector<Statement *> &statementList) : statementList(statementList) {}

CompoundStatement::~CompoundStatement() {
    for (auto statement : statementList) {
        delete statement;
    }
}

StatementClass CompoundStatement::getClass() {
    return StatementClass::COMPOUND_STATEMENT;
}
