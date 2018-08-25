//
// Copyright 2018 - present @adityaatluri
//


#include "Redox/Redox.hpp"

int main() {
    Redox::Var v("v", {dim1, dim2, dim3, dim4});

    // pass std::vector<int> of indices the reduction happens
    v.Reduce();

    Redox::Buffer<float> buff(1024);
    auto out = buff.Reduce();

}