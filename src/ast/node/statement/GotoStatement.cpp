#include "GotoStatement.h"

#include <utility>

#include <iostream>

GotoStatement::GotoStatement(std::string identifier) : identifier(std::move(identifier)) {}

StatementClass GotoStatement::getClass() {
    return StatementClass::GOTO_STATEMENT;
}
