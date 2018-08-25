//
// Copyright 2018 - present @adityaatluri
//

#pragma once

#include "Redox/Buffer.hpp"
#include "Redox/Visitor.hpp"

namespace Redox {

enum IRNodeId_t {
    NodeId = 0,
    ExprId,
    StmtId,
    IntId,
    FloatId,
    DoubleId,
    IdentId,
    MCallId,
    AddOpId,
    SubOpId,
    MulOpId,
    EqOpId,
    AssignId,
    BlockId,
    VarDecId,
    FuncDecId,
    ExprStmtId
};

class Node {
public:
    virtual ~Node() {}
    static inline bool classof(Node const*) { return true; }
    virtual IRNodeId_t getNodeId() const = 0;
    virtual void accept(Visitor& v) const = 0;
};

class BaseStmtNode : public Node {
};

} // end namespace Redox