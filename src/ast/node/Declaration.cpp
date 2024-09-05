#include "Declaration.h"

Declaration::Declaration(int lineNumber, int columnNumber) : Node(lineNumber, columnNumber) {}

void Declaration::accept(Visitor *visitor) {
    visitor->visit(this);
}
