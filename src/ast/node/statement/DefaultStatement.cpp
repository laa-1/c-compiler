#include "DefaultStatement.h"

DefaultStatement::DefaultStatement(int lineNumber, int columnNumber, Statement *statement) : Statement(lineNumber, columnNumber), statement(statement) {}

DefaultStatement::~DefaultStatement() {
    delete statement;
}

StatementClass DefaultStatement::getClass() {
    return StatementClass::DEFAULT_STATEMENT;
}