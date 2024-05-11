#include "DefaultStatement.h"

DefaultStatement::DefaultStatement(Statement *statement) : statement(statement) {}

DefaultStatement::~DefaultStatement() {
    delete statement;
}

StatementClass DefaultStatement::getClass() {
    return StatementClass::DEFAULT_STATEMENT;
}
