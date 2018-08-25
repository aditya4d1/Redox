//
// Copyright 2018 - present @adityaatluri
//

#include "Redox/Buffer.hpp"
#include <iostream>
#include <memory>

using namespace Redox;
using namespace Redox::Internal;

std::unique_ptr<ExprAST> ast;

template<>
Buffer<float> Buffer<float>::Reduce() {
    std::cout << m_num_elements << std::endl;
    Buffer<float> buff(1);
    auto red = std::make_unique<ReduceExprAST>(expr);
    red.get()->codegen();
    ast = std::move(red);
    return buff;
}