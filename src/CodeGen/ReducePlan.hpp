//
// Copyright 2018 - present @adityaatluri
//

#pragma once

#include <cstdlib>

struct ReducePlan_t {
    unsigned num_workitems;
    unsigned num_workgroups;
    size_t num_elements;
    unsigned vectorize_value;
};
