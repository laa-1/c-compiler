#include "SwitchStatement.h"

#include <iostream>

SwitchStatement::SwitchStatement(Expression *expression, Statement *body) : expression(expression), body(body) {}

SwitchStatement::~SwitchStatement() {
    delete expression;
    delete body;
}

StatementClass SwitchStatement::getClass() {
    return StatementClass::SWITCH_STATEMENT;
}
