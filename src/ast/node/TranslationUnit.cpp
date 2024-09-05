#include "TranslationUnit.h"

TranslationUnit::TranslationUnit(int lineNumber, int columnNumber, const std::vector<Declaration *> &declarationList) : Node(lineNumber, columnNumber), declarationList(declarationList) {}
TranslationUnit::~TranslationUnit() {
    for (auto declaration : declarationList) {
        delete declaration;
    }
}

void TranslationUnit::accept(Visitor *visitor) {
    visitor->visit(this);
}
