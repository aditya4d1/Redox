//
// Copyright 2018 - present @adityaatluri
//

#pragma once

#include "Redox/Utils.hpp"

namespace Redox {

namespace Internal {

class ExprAST {
public:
    virtual ~ExprAST() {}
    virtual void codegen() = 0;
};

class BufferExprAST: public ExprAST {
    size_t m_num_elements;
public:
    BufferExprAST(size_t num_elements) : m_num_elements(num_elements) {}
    void codegen() override;
};

class ReduceExprAST: public ExprAST {
    ExprAST* m_input;
public:
    ReduceExprAST(ExprAST* input) : m_input(input) {}
    void codegen() override;
};

}

}
