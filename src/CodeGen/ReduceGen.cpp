//
// Copyright 2018 - present @adityaatluri
//

#include "UtilsLLVM.hpp"
#include "Redox/IR.hpp"
#include "ReduceGen.hpp"

#include <iostream>

namespace llvm {
  extern "C" void LLVMInitializeAMDGPUTargetInfo();
  extern "C" void LLVMInitializeAMDGPUTargetInfo();
  extern "C" void LLVMInitializeAMDGPUTarget();
  extern "C" void LLVMInitializeAMDGPUTargetMC();
  extern "C" void LLVMInitializeAMDGPUAsmParser();
  extern "C" void LLVMInitializeAMDGPUAsmPrinter();
}


using namespace llvm::sys;
using namespace llvm;


const auto workitem_intrin_id_x = llvm::Intrinsic::amdgcn_workitem_id_x;
const auto workgroup_intrin_id_x = llvm::Intrinsic::amdgcn_workgroup_id_x;
const auto barrier_intrin = llvm::Intrinsic::amdgcn_s_barrier;

const std::string reduce_float_16_x1 = \
"\
v_add_f32 $1, $0, $0 row_shr:1 bound_ctrl:0 \
v_add_f32 $1, $0, $1 row_shr:2 bound_ctrl:0 \
v_add_f32 $1, $0, $1 row_shr:3 bound_ctrl:0 \
v_nop \
v_nop \
v_add_f32 $1, $1, $1 row_shr:4 bank_mask:0xe \
v_nop \
v_nop \
v_add_f32 $1, $1, $1 row_shr:8 bank_mask:0xc \
v_nop \
v_nop \
v_add_f32 $1, $1, $1 row_bcast:15 row_mask:0xa \
v_nop \
v_vop \
v_add_f32 $1, $1, $1 row_bcast:16 row_mask:0xc \
";


//
// Create Kernel header including:
// Name, Args
//
llvm::Function* MakeKernelHeader(LLVMContext& context, Module* module, IRBuilder<>& builder, std::string name, unsigned vector_size) {
    Type* arg_type = llvm::VectorType::get(llvm::Type::getFloatTy(context), vector_size);
    std::vector<Type*> args_type(2);
    args_type[0] = llvm::PointerType::get(arg_type, 1);
    args_type[1] = llvm::PointerType::get(llvm::Type::getFloatTy(context), 1);

    std::vector<Value*> args_value(2);

    FunctionType* kernel_func_type;
    Function* kernel_func;

    kernel_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), args_type, false);

    kernel_func = llvm::Function::Create(kernel_func_type, llvm::Function::ExternalLinkage, name, module);

    kernel_func->setCallingConv(llvm::CallingConv::AMDGPU_KERNEL);

    auto entry = llvm::BasicBlock::Create(context, "", kernel_func);
    builder.SetInsertPoint(entry);

    return kernel_func;
}

llvm::Value* GetWorkitemId(Module* module, IRBuilder<>& builder) {
    return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workitem_intrin_id_x), {});
}

llvm::Value* GetWorkgroupId(Module* module, IRBuilder<>& builder) {
    return builder.CreateCall(llvm::Intrinsic::getDeclaration(module, workgroup_intrin_id_x), {});
}

void CreateBarrier(Module* module, IRBuilder<>& builder) {
    builder.CreateCall(llvm::Intrinsic::getDeclaration(module, barrier_intrin), {});
}

llvm::Value* GetLDSPointer(LLVMContext &context, Module* module, IRBuilder<>& builder, unsigned num_workitems, unsigned vectorize_value) {
    Type* lds_element_type = llvm::VectorType::get(llvm::Type::getFloatTy(context), vectorize_value);
    Type* lds_type = llvm::ArrayType::get(lds_element_type, num_workitems);
    GlobalVariable* lds_var = new GlobalVariable(*module, lds_type, false, llvm::GlobalValue::PrivateLinkage, llvm::UndefValue::get(lds_type), std::string("lds") + std::to_string(num_workitems), 0, llvm::GlobalValue::ThreadLocalMode::NotThreadLocal, 3);
    lds_var->setAlignment(16);
    lds_var->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::None);
    return builder.CreateBitCast(lds_var, llvm::PointerType::get(lds_element_type, 3));
}

//
void GenerateReduceKernels(ReducePlan_t plan) {
    std::string name = std::string("main") + std::to_string(plan.vectorize_value);
    LLVMContext context;
    Module* module = new Module("top", context);
    IRBuilder<> builder(context);

    auto kernel_func = MakeKernelHeader(context, module, builder, name, plan.vectorize_value);

    std::vector<llvm::Value*> args_value;

    for(auto& arg: kernel_func->args()) {
        args_value.push_back(&arg);
    }

    auto input_data = args_value[0];
    auto output_data = args_value[1];

    auto workitem_idx = GetWorkitemId(module, builder);
    auto workgroup_idx = GetWorkgroupId(module, builder);

    auto blockSize = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), plan.num_workitems);
    auto blockSizex2 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), plan.num_workitems * 2);

    auto gridSize = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), plan.num_workitems * 2 * plan.num_workgroups);

    auto index = builder.CreateAdd(builder.CreateMul(workgroup_idx, blockSizex2), workitem_idx);

    auto lds = GetLDSPointer(context, module, builder, plan.num_workitems, plan.vectorize_value);

// should be in while loop, do later depending on the number of registers produced
/*
    auto input_ptr = builder.CreateGEP(input_data, index);

    llvm::Value* val = builder.CreateLoad(input_ptr);

    input_ptr = builder.CreateGEP(input_data, builder.CreateAdd(index, blockSize));

    val = builder.CreateFAdd(builder.CreateLoad(input_ptr), val);

    input_ptr = builder.CreateGEP(input_data, builder.CreateAdd(index, gridSize));
*/

// load global memory
    auto global_gep_a = builder.CreateGEP(input_data, workitem_idx);
    auto global_gep_b = builder.CreateGEP(output_data, workgroup_idx);

    auto lds_gep_store = builder.CreateGEP(lds, workitem_idx);

    llvm::Value* r_data = builder.CreateLoad(global_gep_a);
    builder.CreateStore(r_data, lds_gep_store);
    CreateBarrier(module, builder);

// forward declare all variables to be used for conditional code gen
    llvm::BasicBlock *ThenBB, *ElseBB, *MergeBB;
    llvm::Value *CondV, *wid, *accum, *ds_data, *wid_cmp, *lds_gep_load;
    llvm::PHINode* PHI;

    unsigned num_workitems = plan.num_workitems / 2;
        wid_cmp = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), llvm::APInt(32, uint64_t(num_workitems)));
//  initialize basic blocks for different conditional blocks
        CondV = builder.CreateICmpULT(workitem_idx, wid_cmp);
        ThenBB = llvm::BasicBlock::Create(context, "", kernel_func);
        ElseBB = llvm::BasicBlock::Create(context);
        MergeBB = llvm::BasicBlock::Create(context);

        builder.CreateCondBr(CondV, ThenBB, ElseBB);

        builder.SetInsertPoint(ThenBB);
        wid = builder.CreateAdd(workitem_idx, wid_cmp);
        lds_gep_load = builder.CreateGEP(lds, wid);
        ds_data = builder.CreateLoad(lds_gep_load);
        accum = builder.CreateFAdd(r_data, ds_data);
        builder.CreateStore(accum, lds_gep_store);

        builder.CreateBr(MergeBB);
        ThenBB = builder.GetInsertBlock();

        kernel_func->getBasicBlockList().push_back(ElseBB);
        builder.SetInsertPoint(ElseBB);


        builder.CreateBr(MergeBB);

        ElseBB = builder.GetInsertBlock();

        kernel_func->getBasicBlockList().push_back(MergeBB);
        builder.SetInsertPoint(MergeBB);

        PHI = builder.CreatePHI(accum->getType(), 2);

        PHI->addIncoming(accum, ThenBB);
        PHI->addIncoming(r_data, ElseBB);

        CreateBarrier(module, builder);

    size_t pow_2 = num_workitems / 2;

// you assume all the kernels are launched with block size = 1024
    for(size_t pow_2 = num_workitems / 2; pow_2 >= 64; pow_2 = pow_2 / 2) {
        wid_cmp = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), llvm::APInt(32, uint64_t(pow_2)));
//  initialize basic blocks for different conditional blocks
        CondV = builder.CreateICmpULT(workitem_idx, wid_cmp);
        ThenBB = llvm::BasicBlock::Create(context, "", kernel_func);
        ElseBB = llvm::BasicBlock::Create(context);
        MergeBB = llvm::BasicBlock::Create(context);

        builder.CreateCondBr(CondV, ThenBB, ElseBB);

        builder.SetInsertPoint(ThenBB);
        wid = builder.CreateAdd(workitem_idx, wid_cmp);
        lds_gep_load = builder.CreateGEP(lds, wid);
        ds_data = builder.CreateLoad(lds_gep_load);
        accum = builder.CreateFAdd(PHI, ds_data);
        builder.CreateStore(accum, lds_gep_store);

        builder.CreateBr(MergeBB);
        ThenBB = builder.GetInsertBlock();

        kernel_func->getBasicBlockList().push_back(ElseBB);
        builder.SetInsertPoint(ElseBB);


        builder.CreateBr(MergeBB);

        ElseBB = builder.GetInsertBlock();

        kernel_func->getBasicBlockList().push_back(MergeBB);
        builder.SetInsertPoint(MergeBB);

        PHI = builder.CreatePHI(ds_data->getType(), 2);

        PHI->addIncoming(accum, ThenBB);
        PHI->addIncoming(r_data, ElseBB);
        CreateBarrier(module, builder);

    }



//
// for dpp reduce, need to handle different wavefront size
// differently 
//


//
// if(tid == 0) output_data[blockIdx.x] = accum;
//

    wid_cmp = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), llvm::APInt(32, uint64_t(0)));
    
    CondV = builder.CreateICmpEQ(wid_cmp, workgroup_idx);
    ThenBB = llvm::BasicBlock::Create(context, "", kernel_func);
    ElseBB = llvm::BasicBlock::Create(context);
    MergeBB = llvm::BasicBlock::Create(context);

    builder.CreateCondBr(CondV, ThenBB, ElseBB);

    builder.SetInsertPoint(ThenBB);

    auto output_ptr = builder.CreateGEP(output_data, workgroup_idx);

    std::vector<llvm::Value*> scalars;
    llvm::Value* output_vector = PHI;
    unsigned num_scalars = output_vector->getType()->getVectorNumElements();
    unsigned vector_index = 0;
    llvm::Value* scalar = builder.CreateExtractElement(output_vector, vector_index++);
    while(vector_index < num_scalars) {
        scalar = builder.CreateFAdd(scalar, builder.CreateExtractElement(output_vector, vector_index++));
    }

    builder.CreateStore(scalar, output_ptr);

    builder.CreateBr(MergeBB);
    ThenBB = builder.GetInsertBlock();

    kernel_func->getBasicBlockList().push_back(ElseBB);
    builder.SetInsertPoint(ElseBB);

    builder.CreateBr(MergeBB);

    ElseBB = builder.GetInsertBlock();

    kernel_func->getBasicBlockList().push_back(MergeBB);
    builder.SetInsertPoint(MergeBB);


    builder.CreateRetVoid();

    llvm::SmallString<8> data_ll;
    llvm::raw_svector_ostream dest_ll(data_ll);
    dest_ll.SetUnbuffered();
    std::string print_ll(data_ll.begin(), data_ll.end());
    module->print(errs(), nullptr);
    std::cout<<print_ll<<std::endl;

    auto TargetTriple = std::string("amdgcn-amd-amdhsa-hcc");

    llvm::LLVMInitializeAMDGPUTargetInfo();
    llvm::LLVMInitializeAMDGPUTarget();
    llvm::LLVMInitializeAMDGPUTargetMC();
    llvm::LLVMInitializeAMDGPUAsmParser();
    llvm::LLVMInitializeAMDGPUAsmPrinter();

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    if(!Target){
        std::cout << Error<<std::endl;
        return;
    }

    auto GPU = "gfx900";
    auto Features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = Target->createTargetMachine(TargetTriple, GPU, Features, opt, RM);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(TargetTriple);

    auto FileName = std::string("output_") + std::to_string(plan.num_workitems) + std::string(".s");
    std::error_code EC;
    raw_fd_ostream dest(FileName, EC, sys::fs::F_None);

    llvm::legacy::PassManager pass;
    auto FileType = llvm::TargetMachine::CGFT_AssemblyFile;

    if(targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        errs()<< "TargetMachine can't emit a file of this type";
    }

    pass.run(*module);
    dest.flush();

}