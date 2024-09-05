#include "LabelStatement.h"

#include <utility>

#include <iostream>

LabelStatement::LabelStatement(int lineNumber, int columnNumber, std::string identifier, Statement *statement) : Statement(lineNumber, columnNumber), identifier(std::move(identifier)), statement(statement) {}

LabelStatement::~LabelStatement() {
    delete statement;
}

StatementClass LabelStatement::getClass() {
    return StatementClass::LABEL_STATEMENT;
}