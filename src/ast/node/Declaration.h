#pragma once

#include <string>
#include "Node.h"
#include "../visitor/Visitor.h"

enum class StorageSpecifier {
    TYPEDEF,
    EXTERN,
    STATIC,
    AUTO,
    REGISTER,
};

enum class FunctionSpecifier {
    INLINE,
};

enum class DeclarationClass {
    FUNCTION_DECLARATION,
    FUNCTION_DEFINITION,
    VARIABLE_DECLARATION,
};

class Declaration : public Node {
public:
    Declaration(int lineNumber, int columnNumber) : Node(lineNumber, columnNumber) {}

    ~Declaration() override = default;

    void accept(Visitor *visitor) override {
        visitor->visit(this);
    }

    virtual DeclarationClass getClass() = 0;
};