#include "Type.h"

void Type::accept(Visitor *visitor) {
    visitor->visit(this);
}