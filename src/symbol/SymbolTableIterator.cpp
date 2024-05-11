#include "SymbolTableIterator.h"

SymbolTableIterator::SymbolTableIterator(Scope *rootScope) {
    visitStatusStack.push({rootScope, 0});
}

Symbol *SymbolTableIterator::operator[](const std::string &identifier) {
    return (*visitStatusStack.top().scope)[identifier];
}

void SymbolTableIterator::switchScope() {
    if (visitStatusStack.top().nextChildIndex != visitStatusStack.top().scope->childList.size()) {
        visitStatusStack.push({visitStatusStack.top().scope->childList[visitStatusStack.top().nextChildIndex++], 0});
    } else {
        visitStatusStack.pop();
    }
}
