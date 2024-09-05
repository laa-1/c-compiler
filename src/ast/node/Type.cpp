#include "Type.h"

Type::Type(int lineNumber, int columnNumber) : Node(lineNumber, columnNumber) {}

void Type::accept(Visitor *visitor) {
    visitor->visit(this);
}
