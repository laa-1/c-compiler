#include "DoWhileStatement.h"

#include <iostream>

DoWhileStatement::DoWhileStatement(int lineNumber, int columnNumber, Statement *body, Expression *condition) : Statement(lineNumber, columnNumber), body(body), condition(condition) {}

DoWhileStatement::~DoWhileStatement() {
    delete body;
    delete condition;
}

StatementClass DoWhileStatement::getClass() {
    return StatementClass::DO_WHILE_STATEMENT;
}