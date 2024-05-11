#include "Expression.h"

void Expression::accept(Visitor *visitor) {
    visitor->visit(this);
}