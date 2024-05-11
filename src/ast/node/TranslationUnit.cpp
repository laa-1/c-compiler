#include "TranslationUnit.h"

TranslationUnit::TranslationUnit(const std::vector<Declaration *> &declarationList) : declarationList(declarationList) {}

TranslationUnit::~TranslationUnit() {
    for (auto declaration : declarationList) {
        delete declaration;
    }
}

void TranslationUnit::accept(Visitor *visitor) {
    visitor->visit(this);
}
