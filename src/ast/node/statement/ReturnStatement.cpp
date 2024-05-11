#include "ReturnStatement.h"

#include <iostream>

ReturnStatement::ReturnStatement(Expression *value) : value(value) {}

ReturnStatement::~ReturnStatement() {
    delete value;
}

StatementClass ReturnStatement::getClass() {
    return StatementClass::RETURN_STATEMENT;
}
