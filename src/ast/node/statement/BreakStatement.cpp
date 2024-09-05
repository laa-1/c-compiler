#include "BreakStatement.h"

BreakStatement::BreakStatement(int lineNumber, int columnNumber) : Statement(lineNumber, columnNumber) {}

StatementClass BreakStatement::getClass() {
    return StatementClass::BREAK_STATEMENT;
}

