//
// Contains definitions for KernelGen APIs
//
#pragma once

#include "KernelGenUtil.hpp"

llvm::Function* CreateKernelHeader(llvm::LLVMContext&, llvm::Module*, llvm::IRBuilder<>&, std::string);
