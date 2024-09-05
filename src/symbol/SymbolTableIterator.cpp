#include "SymbolTableIterator.h"

SymbolTableIterator::SymbolTableIterator(Scope *rootScope) {
    visitStatusStack.emplace(rootScope, 0);
}

Symbol *SymbolTableIterator::operator[](const std::string &identifier) {
    return (*visitStatusStack.top().first)[identifier];
}

void SymbolTableIterator::switchScope() {
    if (visitStatusStack.top().second != visitStatusStack.top().first->childList.size()) {
        visitStatusStack.emplace(visitStatusStack.top().first->childList[visitStatusStack.top().second++], 0);
    } else {
        visitStatusStack.pop();
    }
}
