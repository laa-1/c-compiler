#include "Statement.h"

Statement::Statement(int lineNumber, int columnNumber) : Node(lineNumber, columnNumber) {}

void Statement::accept(Visitor *visitor) {
    visitor->visit(this);
}
