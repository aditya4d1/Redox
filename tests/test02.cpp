//
// Copyright 2018 - present @adityaatluri
//


#include "Redox/Redox.hpp"

int main() {
    Redox::Buffer<float> buff(1024);
    auto out = buff.Reduce();
}
