//
//  codegen.h
//  ECFBLanguage
//
//  Created by Enrique on 11/22/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#ifndef codegen_h
#define codegen_h

#include <stack>
#include <typeinfo>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitstream/BitstreamReader.h>
#include <llvm/Bitstream/BitstreamWriter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

class NBlock;

static LLVMContext ecfbContext;

class CodeGenBlock {
public:
    BasicBlock *block;
    std::map<std::string, Value*> locals;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    Function* mainFunction;
    
public:
    Module* module;
    CodeGenContext() {
        module = new Module("main", ecfbContext);
    }
    
    void generateCode(NBlock& block);
    GenericValue runCode();
    
    std::map<std::string, Value*>& locals() {
        return blocks.top()->locals;
    }
    
    BasicBlock* currentBlock() {
        return blocks.top()->block;
    }
    
    void pushBlock(BasicBlock* block) {
        blocks.push(new CodeGenBlock());
        blocks.top()->block = block;
    }
    
    void popBlock() {
        CodeGenBlock* top = blocks.top();
        blocks.pop();
        delete top;
    }
};

#endif /* codegen_h */