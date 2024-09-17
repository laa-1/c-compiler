#pragma once

#include "../visitor/Visitor.h"

class Node {
public:
    int lineNumber;
    int columnNumber;

public:
    Node(int lineNumber, int columnNumber) : lineNumber(lineNumber), columnNumber(columnNumber) {}

    virtual ~Node() = default;
    virtual void accept(Visitor *visitor) = 0;
};
