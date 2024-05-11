#include "Statement.h"

void Statement::accept(Visitor *visitor) {
    visitor->visit(this);
}
