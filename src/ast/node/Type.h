#pragma once

#include <string>
#include "Node.h"
#include "../visitor/Visitor.h"

enum class TypeQualifier {
    CONST,
    RESTRICT,
    VOLATILE,
};

enum class BaseType {
    VOID,
    CHAR,
    SHORT,
    INT,
    LONG_INT,
    LONG_LONG_INT,
    UNSIGNED_CHAR,
    UNSIGNED_SHORT,
    UNSIGNED_INT,
    UNSIGNED_LONG_INT,
    UNSIGNED_LONG_LONG_INT,
    FLOAT,
    DOUBLE,
};

enum class TypeClass {
    ARRAY_TYPE,
    FUNCTION_TYPE,
    POINTER_TYPE,
    SCALAR_TYPE,
};

class Type : public Node {
public:
    Type(int lineNumber, int columnNumber);
    ~Type() override = default;
    void accept(Visitor *visitor) override;
    virtual TypeClass getClass() = 0;
    virtual Type *clone() = 0;
};