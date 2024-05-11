#pragma once

#include <vector>
#include "../Statement.h"
#include "../Declaration.h"

class DeclarationStatement : public Statement {
public:
    std::vector<Declaration *> declarationList;

    explicit DeclarationStatement(const std::vector<Declaration *> &declarationList);
    ~DeclarationStatement() override;
    StatementClass getClass() override;
};
