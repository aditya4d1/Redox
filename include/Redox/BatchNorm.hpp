//
// Copyright 2018 - @adityaatluri
//
#pragma once

#include "Tensor.hpp"

namespace Redox {

struct BatchNormPlan_t {
    size_t num_workitems;
    size_t num_workgroups_x;
    size_t num_workgroups_y;
};

template<typename T>
void BatchNorm(Tensor<T> input, Tensor<T> mean, Tensor<T> variance);

void CreateBatchNormKernel(BatchNormPlan_t plan);

}
