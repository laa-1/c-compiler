#include "ForStatement.h"

#include <iostream>

ForStatement::ForStatement(const std::vector<Declaration *> &initDeclarationList, Expression *initExpression, Expression *condition, Expression *updateExpression, Statement *body) : declarationList(initDeclarationList), init(initExpression), condition(condition), update(updateExpression), body(body) {}

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

