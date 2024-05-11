#include "CaseStatement.h"

#include <iostream>

CaseStatement::CaseStatement(int value, Statement *statement) : value(value), statement(statement) {}

CaseStatement::~CaseStatement() {
    delete statement;
}

StatementClass CaseStatement::getClass() {
    return StatementClass::CASE_STATEMENT;
}
