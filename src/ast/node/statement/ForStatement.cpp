#include "ForStatement.h"

#include <iostream>

ForStatement::ForStatement(int lineNumber, int columnNumber, const std::vector<Declaration *> &declarationList, Expression *init, Expression *condition, Expression *update, Statement *body) : Statement(lineNumber, columnNumber), declarationList(declarationList), init(init), condition(condition), update(update), body(body) {}

ForStatement::~ForStatement() {
    for (auto declaration : declarationList) {
        delete declaration;
    }
    delete init;
    delete condition;
    delete update;
    delete body;
}

StatementClass ForStatement::getClass() {
    return StatementClass::FOR_STATEMENT;
}

