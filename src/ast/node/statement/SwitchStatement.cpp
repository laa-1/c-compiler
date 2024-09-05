#include "SwitchStatement.h"

#include <iostream>

SwitchStatement::SwitchStatement(int lineNumber, int columnNumber, Expression *expression, Statement *body) : Statement(lineNumber, columnNumber), expression(expression), body(body) {}

SwitchStatement::~SwitchStatement() {
    delete expression;
    delete body;
}

StatementClass SwitchStatement::getClass() {
    return StatementClass::SWITCH_STATEMENT;
}