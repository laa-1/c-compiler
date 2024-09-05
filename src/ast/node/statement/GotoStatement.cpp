#include "GotoStatement.h"

#include <utility>

#include <iostream>

GotoStatement::GotoStatement(int lineNumber, int columnNumber, const std::string &identifier) : Statement(lineNumber, columnNumber), identifier(identifier) {}

StatementClass GotoStatement::getClass() {
    return StatementClass::GOTO_STATEMENT;
}