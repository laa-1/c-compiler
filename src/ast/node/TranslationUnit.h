#pragma once

#include <vector>
#include <string>
#include "Node.h"
#include "../visitor/Visitor.h"
#include "Declaration.h"

class TranslationUnit : public Node {
public:
    std::vector<Declaration *> declarationList;

    TranslationUnit(int lineNumber, int columnNumber, const std::vector<Declaration *> &declarationList) : Node(lineNumber, columnNumber), declarationList(declarationList) {}

    ~TranslationUnit() override {
        for (auto declaration : declarationList) {
            delete declaration;
        }
    }

    void accept(Visitor *visitor) override {
        visitor->visit(this);
    }

};
