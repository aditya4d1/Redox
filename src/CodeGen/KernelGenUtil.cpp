#include "KernelGen.hpp"

using namespace llvm;

const auto workitem_intrin_id_x = Intrinsic::amdgcn_workitem_id_x;
const auto workitem_intrin_id_y = Intrinsic::amdgcn_workitem_id_y;
const auto workitem_intrin_id_z = Intrinsic::amdgcn_workitem_id_z;

const auto workgroup_intrin_id_x = Intrinsic::amdgcn_workgroup_id_x;
const auto workgroup_intrin_id_y = Intrinsic::amdgcn_workgroup_id_y;
const auto workgroup_intrin_id_z = Intrinsic::amdgcn_workgroup_id_z;

const auto barrier_intrin = Intrinsic::amdgcn_s_barrier;

llvm::Value* GetWorkitemId(llvm::Module* module, llvm::IRBuilder<>& builder, int index) {
    switch(index) {
        case 0: {
            return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workitem_intrin_id_x), {});
        }
        case 1: {
            return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workitem_intrin_id_y), {});
        }
        case 2: {
            return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workitem_intrin_id_z), {});
        }
    }
}

llvm::Value* GetWorkgroupId(llvm::Module* module, llvm::IRBuilder<>& builder, int index) {
    switch(index) {
        case 0: {
            return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workgroup_intrin_id_x), {});
        }
        case 1: {
            return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workgroup_intrin_id_y), {});
        }
        case 2: {
            return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workgroup_intrin_id_z), {});
        }
    }
}

void CreateBarrier(llvm::Module* module, llvm::IRBuilder<>& builder) {
    builder.CreateCall(llvm::Intrinsic::getDeclaration(module, barrier_intrin), {});
}

std::string GetKernelName(std::string prefix, Redox::BatchNormPlan_t plan) {
    return prefix + std::string("_") + std::to_string(plan.num_workitems) + std::string("_") + std::to_string(plan.num_workgroups_x) + std::string("_") + std::to_string(plan.num_workgroups_y);
}
