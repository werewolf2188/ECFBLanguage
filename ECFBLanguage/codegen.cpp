//
//  codegen.cpp
//  ECFBLanguage
//
//  Created by Enrique on 11/22/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include <stdio.h>
#include "semantics.hpp"
#include "codegen.hpp"
#include "parser.hpp"

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock &block) {
    std::cout << "Generating code..." << std::endl;
   
    ArrayRef<Type*> argTypes;
    FunctionType *fType = FunctionType::get(Type::getVoidTy(ecfbContext), argTypes, false);
    mainFunction = Function::Create(fType, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(ecfbContext, "entry", mainFunction);
    
    /* Push a new variable/block context */
    pushBlock(bblock);
    block.codeGen(*this); /* Emit bytecode for the top level block */
    ReturnInst::Create(ecfbContext, bblock);
    popBlock();
    
    /* Print the bytecode in a human-readable format to see if our program compiled properly
     */
    std::cout << "Code has been generated." << std::endl;
    legacy::PassManager pm;
    pm.add(createPrintModulePass(outs()));
    pm.run(*module);
    
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
    std::cout << "Running code..." << std::endl;
    
    ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).create();
    ee->finalizeObject();
    ArrayRef<GenericValue> noargs;
    
    GenericValue v = ee->runFunction(mainFunction, noargs);
    std::cout << "Code was run." << std::endl;
    return v;
}

/* Returns an LLVM type based on the identifier */

static Type* typeOf(const NIdentifier& type) {
    if (type.name.compare("int") == 0) {
        return Type::getInt64Ty(ecfbContext);
    } else if (type.name.compare("double") == 0) {
        return Type::getDoubleTy(ecfbContext);
    } else if (type.name.compare("boolean") == 0) {
        return Type::getInt1Ty(ecfbContext);
    }
    return Type::getVoidTy(ecfbContext);
}

/* -- Code Generation -- */

Value * Node::codeGen(CodeGenContext &context) {
    return NULL;
}

Value * NInteger::codeGen(CodeGenContext& context) {
    std::cout << "Creating integer: " << value << std::endl;
    return ConstantInt::get(Type::getInt64Ty(ecfbContext), value, true);
}

Value * NDouble::codeGen(CodeGenContext& context) {
    std::cout << "Creating double: " << value << std::endl;
    return ConstantFP::get(Type::getDoubleTy(ecfbContext), value);
}

Value* NBoolean::codeGen(CodeGenContext& context) {
    std::cout << "Creating boolean: " << value << std::endl;
    return ConstantInt::get(Type::getInt1Ty(ecfbContext), value);
}

Value * NIdentifier::codeGen(CodeGenContext& context) {
    std::cout << "Creating identifier reference: " << name << std::endl;
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << std::endl;
        return NULL;
    }
    return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

Value * NMethodCall::codeGen(CodeGenContext& context) {
    Function *function = context.module->getFunction(id.name.c_str());
    if (function == NULL) {
        std::cerr << "No such function " << id.name << std::endl;
    }
    std::vector<Value *> args;
    for (ExpressionIterator it = arguments.begin(); it != arguments.end(); it++) {
        args.push_back((**it).codeGen(context));
    }
    CallInst *call = CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
    std::cout << "Creating method call: " << id.name << std::endl;
    return call;
}

Value * NBinaryOperator::codeGen(CodeGenContext& context) {
    std::cout << "Creating binary operation " << op << std::endl;
    Instruction::BinaryOps instr;
    ICmpInst::Predicate cmpPred;
    switch (op) {
        case TPLUS: instr = this->resultingType == TDOUBLE ? Instruction::FAdd : Instruction::Add; goto math;
        case TMINUS: instr = this->resultingType == TDOUBLE ? Instruction::FSub : Instruction::Sub; goto math;
        case TMUL: instr = this->resultingType == TDOUBLE ? Instruction::FMul : Instruction::Mul; goto math;
        case TDIV: instr = this->resultingType == TDOUBLE ? Instruction::FDiv : Instruction::SDiv; goto math;
            /* TODO comparison */
        case TCEQ: cmpPred = ICmpInst::ICMP_EQ; goto comp;
        case TCNE: cmpPred = ICmpInst::ICMP_NE; goto comp;
        case TCLT: cmpPred = ICmpInst::ICMP_ULT; goto comp;
        case TCLE: cmpPred = ICmpInst::ICMP_ULE; goto comp;
        case TCGT: cmpPred = ICmpInst::ICMP_UGT; goto comp;
        case TCGE: cmpPred = ICmpInst::ICMP_UGE; goto comp;
    }
    return NULL;
    math:
    return BinaryOperator::Create(instr, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
    comp:
    return new ICmpInst(*context.currentBlock(), cmpPred, lhs.codeGen(context), rhs.codeGen(context));
}

Value * NAssignment::codeGen(CodeGenContext& context) {
    std::cout << "Creating assignment for " << lhs.name << std::endl;
    if (context.locals().find(lhs.name) == context.locals().end()) {
        std::cerr << "Undeclared variable" << lhs.name << std::endl;
        return NULL;
    }
    return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

Value * NBlock::codeGen(CodeGenContext& context) {
    Value *last = NULL;
    for (StatementIterator it = statements.begin(); it != statements.end(); it++) {
        std::cout << "Generating code for " << typeid(**it).name() << std::endl;
        last = (**it).codeGen(context);
    }
    std::cout << "Creating block" << std::endl;
    return last;
}

Value * NExpressionStatement::codeGen(CodeGenContext& context) {
    std::cout << "Generating code for " << typeid(expression).name() << std::endl;
    return expression.codeGen(context);
}

Value* NReturnStatement::codeGen(CodeGenContext &context) {
    std::cout << "Generating return code for " << typeid(expression).name() << std::endl;
    Value *returnValue = expression.codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

Value * NVariableDeclaration::codeGen(CodeGenContext& context) {
    std::cout << "Creating variable declaration " << type.name << " " << id.name << std::endl;
    
    AllocaInst *alloc = new AllocaInst(typeOf(type), 0 ,id.name.c_str(), context.currentBlock());
    context.locals()[id.name] = alloc;
    if (assignmentExpr != NULL) {
        NAssignment assn(id, *assignmentExpr);
        assn.codeGen(context);
    }
    return alloc;
}

Value * NFunctionDeclaration::codeGen(CodeGenContext& context) {
    std::vector<Type *> argTypes;
    for (VariableIterator it = arguments.begin(); it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    
    FunctionType* fType = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
    Function* function = Function::Create(fType, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
    BasicBlock* bblock = BasicBlock::Create(ecfbContext, "entry", function, 0);
    
    context.pushBlock(bblock);
    
    Function::arg_iterator argsValues = function->arg_begin();
    Value* argumentValue;
    
    for (VariableIterator it = arguments.begin(); it != arguments.end(); it++) {
        (**it).codeGen(context);
        
        argumentValue = &*argsValues++;
        argumentValue->setName((*it)->id.name.c_str());
        StoreInst *inst = new StoreInst(argumentValue, context.locals()[(*it)->id.name], false, bblock);
    }
    
    block.codeGen(context);
    ReturnInst::Create(ecfbContext, context.getCurrentReturnValue(), bblock);

    context.popBlock();
    std::cout << "Creating function: " << id.name << std::endl;
    return function;
}
