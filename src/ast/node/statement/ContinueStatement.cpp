#include "ContinueStatement.h"

ContinueStatement::ContinueStatement(int lineNumber, int columnNumber) : Statement(lineNumber, columnNumber) {}

StatementClass ContinueStatement::getClass() {
    return StatementClass::CONTINUE_STATEMENT;
}