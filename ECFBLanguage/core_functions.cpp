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

llvm::Function* createGetSFunction(CodeGenContext& context) {
    std::vector<llvm::Type*> gets_arg_types;
       gets_arg_types.push_back(llvm::Type::getInt8PtrTy(ecfbContext)); //char*

       llvm::FunctionType* gets_type =
           llvm::FunctionType::get(
               llvm::Type::getInt32Ty(ecfbContext), gets_arg_types, false);
    
    llvm::Function *func = llvm::Function::Create(
         gets_type, llvm::Function::ExternalLinkage,
         llvm::Twine("gets"),
         context.module
    );
    
    return func;
}

llvm::Function* createPrintfFunction(CodeGenContext& context)
{
    std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(ecfbContext)); //char*

    llvm::FunctionType* printf_type =
        llvm::FunctionType::get(
            llvm::Type::getInt8PtrTy(ecfbContext), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                context.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void createEchoBooleanFunction(CodeGenContext& context, llvm::Function* printfFn) {
    const char* constTrueValue = "True\n";
    const char* constFalseValue = "False\n";
    
    // Constant values
    llvm::Constant *format_true_const = llvm::ConstantDataArray::getString(ecfbContext, constTrueValue);
    llvm::Constant *format_false_const = llvm::ConstantDataArray::getString(ecfbContext, constFalseValue);
    
    // Global variables holding the values
    llvm::GlobalVariable *true_var =
    new llvm::GlobalVariable(
        *context.module, llvm::ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), strlen(constTrueValue)+1),
        true, llvm::GlobalValue::PrivateLinkage, format_true_const, ".truestring");
    llvm::GlobalVariable *false_var =
    new llvm::GlobalVariable(
        *context.module, llvm::ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), strlen(constFalseValue)+1),
        true, llvm::GlobalValue::PrivateLinkage, format_false_const, ".falsestring");
    
    //Arguments that the function is expecting
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(llvm::Type::getInt1Ty(ecfbContext));
    
    llvm::FunctionType* echo_type =
    llvm::FunctionType::get(
        llvm::Type::getVoidTy(ecfbContext), echo_arg_types, false);
    
    
    //Function echob
    llvm::Function *func = llvm::Function::Create(
         echo_type, llvm::Function::InternalLinkage,
         llvm::Twine("echob"),
         context.module
    );
    
    // Lets get the arguments of echob and rename it
    Function::arg_iterator argsValues = func->arg_begin();
    Value* toPrint = &*argsValues++;
    toPrint->setName("toPrint");
    
    
    // Block echob
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(ecfbContext, "entry", func, 0);
    context.pushBlock(bblock, NULL);
    
    //For the purpose of adding zero at the end of the string, lets create a null constant
    llvm::Constant *zero =
    llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(ecfbContext));
    std::vector<llvm::Constant*> indices;
    indices.push_back(zero);
    indices.push_back(zero);

    llvm::Constant *true_var_ref = llvm::ConstantExpr::getGetElementPtr(
    llvm::ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), strlen(constTrueValue)+1),
        true_var, indices);
    llvm::Constant *false_var_ref = llvm::ConstantExpr::getGetElementPtr(
    llvm::ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), strlen(constTrueValue)+1),
        false_var, indices);
    
    //Lets create the condition for the print
    CmpInst* conditionInst = new ICmpInst(*bblock, ICmpInst::ICMP_EQ, toPrint,  ConstantInt::get(Type::getInt1Ty(ecfbContext), 1));
    // Block echob
    llvm::BasicBlock *trueBlock = llvm::BasicBlock::Create(ecfbContext, "trueBlock", func);
    // Block echob
    llvm::BasicBlock *falseBlock = llvm::BasicBlock::Create(ecfbContext, "falseBlock", func);
    BranchInst::Create(trueBlock, falseBlock, conditionInst, bblock);
    
    std::vector<Value*> true_args;
    true_args.push_back(true_var_ref);
    
    CallInst *true_call = CallInst::Create(printfFn, makeArrayRef(true_args), "", trueBlock);
    ReturnInst::Create(ecfbContext, trueBlock);
    
    std::vector<Value*> false_args;
    false_args.push_back(false_var_ref);
    CallInst *false_call = CallInst::Create(printfFn, makeArrayRef(false_args), "", falseBlock);
    ReturnInst::Create(ecfbContext, falseBlock);
    
    context.popBlock();
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
    context.pushBlock(bblock, NULL);
    
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
    createGetSFunction(context);
    llvm::Function* printfFn = createPrintfFunction(context);
    createEchoFunction(context, printfFn, "%d\n", "echoi", llvm::Type::getInt64Ty(ecfbContext));
    createEchoFunction(context, printfFn, "%f\n", "echod", llvm::Type::getDoubleTy(ecfbContext));
    createEchoBooleanFunction(context, printfFn);
}
