#include "KernelGen.hpp"

#include <vector>
#include <iostream>

using namespace llvm;

//
// we assume input arg is float4, output arg is float
//
Function* CreateKernelHeader(LLVMContext& context, Module* module, IRBuilder<>& builder, std::string name) {
    std::vector<Type*> args_type(2);

    args_type[1] = PointerType::get(VectorType::get(Type::getFloatTy(context), 4), 1);
    args_type[0] = PointerType::get(Type::getFloatTy(context), 1);

    FunctionType* kernel_func_type = FunctionType::get(Type::getVoidTy(context), args_type, false);
    Function* kernel_func = Function::Create(kernel_func_type, Function::ExternalLinkage, name, module);

    kernel_func->setCallingConv(CallingConv::AMDGPU_KERNEL);

    auto entry = BasicBlock::Create(context, "", kernel_func);
    builder.SetInsertPoint(entry);

    return kernel_func;
}
