//
// Copyright 2018 - present @adityaatluri
//

#include "Redox/IR.hpp"
#include "ReducePlan.hpp"
#include "ReduceGen.hpp"
#include <list>

using namespace Redox::Internal;

// unordered set is more appropriate
std::list<std::unique_ptr<ReducePlan_t>> l_plans;

// figure out plan depending on the buffer size
void decidePlan(size_t num_elements) {
    for(unsigned vector_len = 1; vector_len <= 4; vector_len *= 2) {
        auto up_plan = std::make_unique<ReducePlan_t>();
        up_plan.get()->vectorize_value = vector_len;
        size_t num_workitems_required = num_elements / vector_len + (num_elements % vector_len > 0 ? 1 : 0);
        up_plan.get()->num_workitems = num_workitems_required < 1024 ? num_workitems_required : 1024;
        up_plan.get()->num_workgroups = num_workitems_required % up_plan.get()->num_workitems == 0 ? num_workitems_required / up_plan.get()->num_workitems : (num_workitems_required / up_plan.get()->num_workitems) + 1;
        up_plan.get()->num_elements = num_elements;
        l_plans.push_back(std::move(up_plan));
    }
}

void ReduceExprAST::codegen() {
    decidePlan(1024);
    for(auto iter = l_plans.begin(); iter != l_plans.end(); iter++) {
        GenerateReduceKernels(*(iter->get()));
    }
}