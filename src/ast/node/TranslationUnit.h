#pragma once

#include <vector>
#include <string>
#include "Node.h"
#include "../visitor/Visitor.h"
#include "Declaration.h"

/**
 * AST的根节点类。
 * 与其他的Node子类不同，它没有子类，仅作为AST的根节点来使用，相当于一个入口
 */
class TranslationUnit : public Node {
public:
    std::vector<Declaration *> declarationList;

    explicit TranslationUnit(const std::vector<Declaration *> &declarationList);
    ~TranslationUnit() override;
    void accept(Visitor *visitor) override;
};
