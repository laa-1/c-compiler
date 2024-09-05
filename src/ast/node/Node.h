#pragma once

#include "../visitor/Visitor.h"

/**
 * 所有AST节点类的最顶层抽象父类，仅用于以多态形式接受visitor。
 * 其子类是AST节点的第二层抽象父类（TranslationUnit除外），用于在各个AST节点具体子类中实现多态。
 * 其子类的子类是AST节点的具体子类。
 */
class Node {
public:
    int lineNumber;
    int columnNumber;

public:
    Node(int lineNumber, int columnNumber);
    virtual ~Node() = default;
    virtual void accept(Visitor *visitor) = 0;
};
