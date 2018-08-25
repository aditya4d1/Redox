//
// Copyright 2018 - present @adityaatluri
//

#pragma once

#include"llvm/ADT/ArrayRef.h"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Function.h"
#include"llvm/IR/BasicBlock.h"
#include"llvm/IR/InlineAsm.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Constants.h"

#include"llvm/Support/TargetRegistry.h"
#include"llvm/Target/TargetOptions.h"
#include"llvm/Support/CodeGen.h"
#include"llvm/Target/TargetMachine.h"
#include"llvm/IR/LegacyPassManager.h"
#include"llvm/IR/PassManager.h"
#include"llvm/Support/FileSystem.h"
#include"llvm/Support/raw_ostream.h"
