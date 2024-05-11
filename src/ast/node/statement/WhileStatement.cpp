#include "WhileStatement.h"

#include <iostream>

WhileStatement::WhileStatement(Expression *condition, Statement *body) : condition(condition), body(body) {}

WhileStatement::~WhileStatement() {
    delete condition;
    delete body;
}

StatementClass WhileStatement::getClass() {
    return StatementClass::WHILE_STATEMENT;
}
