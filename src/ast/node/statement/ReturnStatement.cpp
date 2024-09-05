#include "ReturnStatement.h"

#include <iostream>

ReturnStatement::ReturnStatement(int lineNumber, int columnNumber, Expression *value) : Statement(lineNumber, columnNumber), value(value) {}

ReturnStatement::~ReturnStatement() {
    delete value;
}

StatementClass ReturnStatement::getClass() {
    return StatementClass::RETURN_STATEMENT;
}