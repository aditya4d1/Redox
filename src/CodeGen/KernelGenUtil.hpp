#pragma once

#include "UtilsLLVM.hpp"
#include "Redox/BatchNorm.hpp"

#include <string>

llvm::Value* GetWorkitemId(llvm::Module* module, llvm::IRBuilder<>& builder, int index);
llvm::Value* GetWorkgroupId(llvm::Module* module, llvm::IRBuilder<>& builder, int index);

void CreateBarrier(llvm::Module* module, llvm::IRBuilder<>& builder);

std::string GetKernelName(std::string prefix, Redox::BatchNormPlan_t plan);
