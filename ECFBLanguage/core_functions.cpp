//
//  core_functions.cpp
//  ECFBLanguage
//
//  Created by Enrique on 11/23/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include <stdio.h>
#include "codegen.hpp"
#include "semantics.hpp"

using namespace std;

extern int yyparse();
extern NBlock* programBlock;

llvm::Function* createPrintfFunction(CodeGenContext& context)
{
    std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(ecfbContext)); //char*

    llvm::FunctionType* printf_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(ecfbContext), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                context.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void createEchoFunction(CodeGenContext& context, llvm::Function* printfFn, const char* constValue, const char *name, Type* expectedType)
{
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(expectedType);

    llvm::FunctionType* echo_type =
        llvm::FunctionType::get(
            llvm::Type::getVoidTy(ecfbContext), echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine(name),
                context.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(ecfbContext, "entry", func, 0);
    context.pushBlock(bblock);
    
    llvm::Constant *format_const = llvm::ConstantDataArray::getString(ecfbContext, constValue);
    llvm::GlobalVariable *var =
        new llvm::GlobalVariable(
            *context.module, llvm::ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), strlen(constValue)+1),
            true, llvm::GlobalValue::PrivateLinkage, format_const, ".str");
    llvm::Constant *zero =
        llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(ecfbContext));

    std::vector<llvm::Constant*> indices;
    indices.push_back(zero);
    indices.push_back(zero);
    llvm::Constant *var_ref = llvm::ConstantExpr::getGetElementPtr(
    llvm::ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), strlen(constValue)+1),
        var, indices);

    std::vector<Value*> args;
    args.push_back(var_ref);

    Function::arg_iterator argsValues = func->arg_begin();
    Value* toPrint = &*argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);
    
    CallInst *call = CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    ReturnInst::Create(ecfbContext, bblock);
    context.popBlock();
}


void createCoreFunctions(CodeGenContext& context) {
    llvm::Function* printfFn = createPrintfFunction(context);
    createEchoFunction(context, printfFn, "%d\n", "echoi", llvm::Type::getInt64Ty(ecfbContext));
    createEchoFunction(context, printfFn, "%f\n", "echod", llvm::Type::getDoubleTy(ecfbContext));
}