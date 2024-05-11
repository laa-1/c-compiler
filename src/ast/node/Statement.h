#pragma once

#include <string>
#include "Node.h"
#include "../visitor/Visitor.h"

enum class StatementClass {
    BREAK_STATEMENT,
    CASE_STATEMENT,
    COMPOUND_STATEMENT,
    CONTINUE_STATEMENT,
    DECLARATION_STATEMENT,
    DEFAULT_STATEMENT,
    DO_WHILE_STATEMENT,
    EXPRESSION_STATEMENT,
    FOR_STATEMENT,
    GOTO_STATEMENT,
    IF_STATEMENT,
    LABEL_STATEMENT,
    RETURN_STATEMENT,
    SWITCH_STATEMENT,
    WHILE_STATEMENT,
};

class Statement : public Node {
public:
    ~Statement() override = default;
    void accept(Visitor *visitor) override;
    virtual StatementClass getClass() = 0;
};
