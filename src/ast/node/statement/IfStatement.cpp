#include "IfStatement.h"

#include <iostream>

IfStatement::IfStatement(int lineNumber, int columnNumber, Expression *condition, Statement *trueBody, Statement *falseBody) : Statement(lineNumber, columnNumber), condition(condition), trueBody(trueBody), falseBody(falseBody) {}

IfStatement::~IfStatement() {
    delete condition;
    delete trueBody;
    delete falseBody;
}

StatementClass IfStatement::getClass() {
    return StatementClass::IF_STATEMENT;
}

