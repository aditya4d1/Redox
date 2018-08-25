#include "Redox/BatchNorm.hpp"

#include "KernelGen.hpp"
#include <iostream>
/**
//
// we load h * w in a workgroup
// we launch n * c workgroups
// c across workgroup dim x
// n across workgroup dim y
//
template<int n, int c, int h, int w>
__global__ void BatchNorm(float* output, float4_t* input) {
    int tx = threadIdx.x;
    int bx = blockIdx.x;
    int by = blockIdx.y;

    auto pin = input + tx + bx * h * w + by * c * h * w;

    float4_t out_val = reduce_in_block(pin);

    auto pout = output + bx + by * n * c;

    *pout = out_val.x + out_val.y + out_val.z + out_val.w;
}
*/

using namespace llvm;
using namespace std;

namespace Redox {
void CreateBatchNormKernel(Redox::BatchNormPlan_t plan) {
    LLVMContext context;
    Module* module = new Module("top", context);
    IRBuilder<> builder(context);

    std::string kernel_name = GetKernelName("mean", plan);

    llvm::Function* kernel = CreateKernelHeader(context, module, builder, kernel_name);

    std::vector<Value*> args_value;

    for(auto& arg: kernel->args()) {
        args_value.push_back(&arg);
    }

    builder.CreateRetVoid();

    SmallString<8> data_ll;
    raw_svector_ostream dest_ll(data_ll);
    dest_ll.SetUnbuffered();
    std::string print_ll(data_ll.begin(), data_ll.end());
    module->print(errs(), nullptr);
    std::cout << print_ll << std::endl;
}
}
