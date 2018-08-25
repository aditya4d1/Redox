//
// Copyright 2018 - @adityaatluri
//

#include "Redox/BatchNorm.hpp"
#include <iostream>

namespace Redox {

template<>
void BatchNorm(Tensor<float> input, Tensor<float> mean, Tensor<float> variance) {
    size_t total_reduction_elements = input.Batches() * input.Height() * input.Width();

    size_t total_reduction_vectors = total_reduction_elements / 4;

    size_t num_workgroups_y = input.Channels();
    size_t num_workitems = 1024;
    size_t num_workgroups_x = total_reduction_vectors / num_workitems;

    std::cout << num_workgroups_y << " " << num_workgroups_x << " " << num_workitems << std::endl;    

    Tensor<float> workspace(1, 1, num_workgroups_x, num_workgroups_y);

    BatchNormPlan_t plan;
    plan.num_workitems = num_workitems;
    plan.num_workgroups_x = num_workgroups_x;
    plan.num_workgroups_y = num_workgroups_y;

    CreateBatchNormKernel(plan);

}

}
