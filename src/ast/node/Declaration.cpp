#include "Declaration.h"

void Declaration::accept(Visitor *visitor) {
    visitor->visit(this);
}