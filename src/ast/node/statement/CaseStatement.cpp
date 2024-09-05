#include "CaseStatement.h"

#include <iostream>

CaseStatement::CaseStatement(int lineNumber, int columnNumber, int value, Statement *statement) : Statement(lineNumber, columnNumber), value(value), statement(statement) {}

CaseStatement::~CaseStatement() {
    delete statement;
}

StatementClass CaseStatement::getClass() {
    return StatementClass::CASE_STATEMENT;
}
