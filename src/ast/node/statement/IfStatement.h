#pragma once

#include "../Statement.h"
#include "../Expression.h"

class IfStatement : public Statement {
public:
    Expression *condition;
    Statement *trueBody;
    Statement *falseBody; // 可以为空

    IfStatement(int lineNumber, int columnNumber, Expression *condition, Statement *trueBody, Statement *falseBody) : Statement(lineNumber, columnNumber), condition(condition), trueBody(trueBody), falseBody(falseBody) {}

    ~IfStatement() override {
        delete condition;
        delete trueBody;
        delete falseBody;
    }

    StatementClass getClass() override {
        return StatementClass::IF_STATEMENT;
    }

};
