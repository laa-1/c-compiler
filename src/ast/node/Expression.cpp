#include "Expression.h"

Expression::Expression(int lineNumber, int columnNumber) : Node(lineNumber, columnNumber) {}

void Expression::accept(Visitor *visitor) {
    visitor->visit(this);
}
