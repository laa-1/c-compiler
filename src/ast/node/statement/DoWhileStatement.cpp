#include "DoWhileStatement.h"

#include <iostream>

DoWhileStatement::DoWhileStatement(Statement *body, Expression *condition) : body(body), condition(condition) {}

DoWhileStatement::~DoWhileStatement() {
    delete body;
    delete condition;
}

StatementClass DoWhileStatement::getClass() {
    return StatementClass::DO_WHILE_STATEMENT;
}
