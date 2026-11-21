#pragma once

#include "Type.h"

// Semantic attributes attached to an AST node after type checking.
// constValue is meaningful only when isConst is true; hasError marks a node
// whose subtree already produced a diagnostic.
struct NodeAttr {
    Type type;
    bool isConst = false;
    int constValue = 0;
    bool hasError = false;
};
