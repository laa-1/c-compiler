#include "WhileStatement.h"

#include <iostream>

WhileStatement::WhileStatement(int lineNumber, int columnNumber, Expression *condition, Statement *body) : Statement(lineNumber, columnNumber), condition(condition), body(body) {}

WhileStatement::~WhileStatement() {
    delete condition;
    delete body;
}

StatementClass WhileStatement::getClass() {
    return StatementClass::WHILE_STATEMENT;
}