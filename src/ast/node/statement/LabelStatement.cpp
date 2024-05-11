#include "LabelStatement.h"

#include <utility>

#include <iostream>

LabelStatement::LabelStatement(std::string identifier, Statement *statement) : identifier(std::move(identifier)), statement(statement) {}

LabelStatement::~LabelStatement() {
    delete statement;
}

StatementClass LabelStatement::getClass() {
    return StatementClass::LABEL_STATEMENT;
}

