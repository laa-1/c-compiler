#include "IfStatement.h"

#include <iostream>

IfStatement::IfStatement(Expression *condition, Statement *trueBody, Statement *falseBody) : condition(condition), trueBody(trueBody), falseBody(falseBody) {}

IfStatement::~IfStatement() {
    delete condition;
    delete trueBody;
    delete falseBody;
}

StatementClass IfStatement::getClass() {
    return StatementClass::IF_STATEMENT;
}
