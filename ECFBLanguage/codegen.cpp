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
#include <sstream>

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock &block) {
    std::cout << "Generating code..." << std::endl;
   
    ArrayRef<Type*> argTypes;
    FunctionType *fType = FunctionType::get(Type::getVoidTy(ecfbContext), argTypes, false);
    mainFunction = Function::Create(fType, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(ecfbContext, "entry", mainFunction);
    
    /* Push a new variable/block context */
    pushBlock(bblock, &block);
    block.codeGen(*this); /* Emit bytecode for the top level block */
    ReturnInst::Create(ecfbContext, this->currentBlock());
    popBlock();
    
    /* Print the bytecode in a human-readable format to see if our program compiled properly
     */
#ifdef DEBUGPRINT
    std::cout << "Code has been generated." << std::endl;
    legacy::PassManager pm;
    pm.add(createPrintModulePass(outs()));
    pm.run(*module);
#endif
    
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
    } else if (type.name.compare("string") == 0) {
        // get the length
        
        return  ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), 255)->getPointerTo();
    }
    return Type::getVoidTy(ecfbContext);
}

/* -- Code Generation -- */

Value * Node::codeGen(CodeGenContext &context) {
    this->generated = true;
    return NULL;
}

Value * NInteger::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating integer: " << value << std::endl;
#endif
    return ConstantInt::get(Type::getInt64Ty(ecfbContext), value, true);
}

Value * NDouble::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating double: " << value << std::endl;
#endif
    return ConstantFP::get(Type::getDoubleTy(ecfbContext), value);
}

Value * NString::codeGen(CodeGenContext &context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating string: " << value << std::endl;
#endif
    const char* cValue = value->c_str();
    size_t size = sizeof(cValue);
    
    llvm::Constant *stringConstant = llvm::ConstantDataArray::getString(ecfbContext, cValue);
    llvm::GlobalVariable *varStr = new llvm::GlobalVariable(*context.module, ArrayType::get(llvm::IntegerType::get(ecfbContext, 8), size+1), true, llvm::GlobalValue::PrivateLinkage, stringConstant, ".varstr");
    return varStr;
}

Value* NBoolean::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating boolean: " << value << std::endl;
#endif
    return ConstantInt::get(Type::getInt1Ty(ecfbContext), (value ? 1 : 0));
}

Value * NIdentifier::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating identifier reference: " << name << std::endl;
#endif
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << std::endl;
        return NULL;
    }
    return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

Value * NMethodCall::codeGen(CodeGenContext& context) {
    this->generated = true;
    Function *function = context.module->getFunction(id.name.c_str());
    if (function == NULL) {
        std::cerr << "No such function " << id.name << std::endl;
    }
    std::vector<Value *> args;
    for (ExpressionIterator it = arguments.begin(); it != arguments.end(); it++) {
        args.push_back((**it).codeGen(context));
    }
    CallInst *call = CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
#ifdef DEBUGPRINT
    std::cout << "Creating method call: " << id.name << std::endl;
#endif
    return call;
}

Value * NBinaryOperator::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating binary operation " << op << std::endl;
#endif
    Instruction::BinaryOps instr;
    ICmpInst::Predicate cmpPred;
    FCmpInst::Predicate cmpFPred;
    // There's a bug in here
    
    bool lhsIsDouble = lhs.resultType(context.currentNBlock()) == TDOUBLE;
    bool rhsIsDouble = rhs.resultType(context.currentNBlock()) == TDOUBLE;
    switch (op) {
        case TPLUS: instr = this->resultingType == TDOUBLE ? Instruction::FAdd : Instruction::Add; goto math;
        case TMINUS: instr = this->resultingType == TDOUBLE ? Instruction::FSub : Instruction::Sub; goto math;
        case TMUL: instr = this->resultingType == TDOUBLE ? Instruction::FMul : Instruction::Mul; goto math;
        case TDIV: instr = this->resultingType == TDOUBLE ? Instruction::FDiv : Instruction::SDiv; goto math;
        case TREMAIN: instr = this->resultingType == TDOUBLE ? Instruction::FRem : Instruction::SRem; goto math;
            /* TODO comparison */
        case TCEQ:
            if (lhsIsDouble && rhsIsDouble) {
                cmpFPred = FCmpInst::FCMP_OEQ; goto compFlt;
            } else {
                cmpPred = ICmpInst::ICMP_EQ; goto compInt;
            }
            
        case TCNE:
            if (lhsIsDouble && rhsIsDouble) {
                cmpFPred = FCmpInst::FCMP_ONE; goto compFlt;
            } else {
                cmpPred = ICmpInst::ICMP_NE; goto compInt;
            }
        case TCLT:
            if (lhsIsDouble && rhsIsDouble) {
                cmpFPred = FCmpInst::FCMP_OLT; goto compFlt;
            } else {
                cmpPred = ICmpInst::ICMP_ULT; goto compInt;
            }
        case TCLE:
            if (lhsIsDouble && rhsIsDouble) {
                cmpFPred = FCmpInst::FCMP_OLE; goto compFlt;
            } else {
                cmpPred = ICmpInst::ICMP_ULE; goto compInt;
            }
        case TCGT:
            if (lhsIsDouble && rhsIsDouble) {
                cmpFPred = FCmpInst::FCMP_OGT; goto compFlt;
            } else {
                cmpPred = ICmpInst::ICMP_UGT; goto compInt;
            }
        case TCGE:
            if (lhsIsDouble && rhsIsDouble) {
                cmpFPred = FCmpInst::FCMP_OGE; goto compFlt;
            } else  {
                cmpPred = ICmpInst::ICMP_UGE; goto compInt;
            }
        case TAND:
            instr = Instruction::And; goto math;
        case TOR:
            instr = Instruction::Or; goto math;
    }
    
    return NULL;
    math:
    return BinaryOperator::Create(instr, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
    compInt:
    return new ICmpInst(*context.currentBlock(), cmpPred, lhs.codeGen(context), rhs.codeGen(context));
    compFlt:
    return new FCmpInst(*context.currentBlock(), cmpFPred, lhs.codeGen(context), rhs.codeGen(context));
}

Value * NUnaryOperator::codeGen(CodeGenContext &context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating unary operation " << op << std::endl;
#endif
    switch (op) {
        case TMINUS:
            if (this->resultingType == TDOUBLE) {
                return UnaryOperator::CreateFNeg(rhs.codeGen(context), "", context.currentBlock());
            } else if (this->resultingType == TINTEGER) {
                Value * minusOne = ConstantInt::get(Type::getInt64Ty(ecfbContext), -1, true);
                Value * bOp = BinaryOperator::Create(Instruction::Mul, minusOne, rhs.codeGen(context), "", context.currentBlock());
                return bOp;
            }
            break;
        case TNOT:
            Value * one = ConstantInt::get(Type::getInt1Ty(ecfbContext), 1);
            Value * bOp = BinaryOperator::Create(Instruction::Xor, one, rhs.codeGen(context), "", context.currentBlock());
            return bOp;
    }
    return NULL;
}

Value * NDataConversion::codeGen(CodeGenContext &context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating Data Conversion for " << type.name << std::endl;
#endif
    if (resultingType == TDOUBLE && previousType == TINTEGER) {
        return new SIToFPInst(rhs.codeGen(context), Type::getDoubleTy(ecfbContext), "conv", context.currentBlock());
    } else if (resultingType == TINTEGER && previousType == TDOUBLE) {        
        return new FPToSIInst(rhs.codeGen(context), Type::getInt64Ty(ecfbContext), "conv", context.currentBlock());
    }
    
    return rhs.codeGen(context);
}

Value * NAssignment::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating assignment for " << lhs.name << std::endl;
#endif
    if (context.locals().find(lhs.name) == context.locals().end()) {
        std::cerr << "Undeclared variable" << lhs.name << std::endl;
        return NULL;
    }
    return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

Value * NBlock::codeGen(CodeGenContext& context) {
    this->generated = true;
    Value *last = NULL;
    for (StatementIterator it = statements.begin(); it != statements.end(); it++) {
#ifdef DEBUGPRINT
        std::cout << "Generating code for " << typeid(**it).name() << std::endl;
#endif
        if (!(**it).wasGenerated()) {
            last = (**it).codeGen(context);
        }
    }
#ifdef DEBUGPRINT
    std::cout << "Creating block" << std::endl;
#endif
    return last;
}

Value * NExpressionStatement::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Generating code for " << typeid(expression).name() << std::endl;
#endif
    return expression.codeGen(context);
}

Value* NReturnStatement::codeGen(CodeGenContext &context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Generating return code for " << typeid(expression).name() << std::endl;
#endif
    Value *returnValue = expression.codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

Value * NVariableDeclaration::codeGen(CodeGenContext& context) {
    this->generated = true;
#ifdef DEBUGPRINT
    std::cout << "Creating variable declaration " << type.name << " " << id.name << std::endl;
#endif
    if (context.isMain()) {
        GlobalVariable *variable = new GlobalVariable(*context.module,
                                                      typeOf(type),
                                                      false,
                                                      GlobalValue::CommonLinkage,
                                                      Constant::getNullValue(typeOf(type)),
                                                      id.name.c_str());
        context.locals()[id.name] = variable;
        if (assignmentExpr != NULL) {
            NAssignment assn(id, *assignmentExpr);
            assn.codeGen(context);
        }
        return variable;
    } else {
        AllocaInst *alloc = new AllocaInst(typeOf(type), 0 ,id.name.c_str(), context.currentBlock());
        context.locals()[id.name] = alloc;
        if (assignmentExpr != NULL) {
            NAssignment assn(id, *assignmentExpr);
            assn.codeGen(context);
        }
        return alloc;
    }
}

Value * NFunctionDeclaration::codeGen(CodeGenContext& context) {
    this->generated = true;
    std::vector<Type *> argTypes;
    for (VariableIterator it = arguments.begin(); it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    
    FunctionType* fType = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
    Function* function = Function::Create(fType, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
    BasicBlock* bblock = BasicBlock::Create(ecfbContext, "entry", function, 0);
    
    context.pushBlock(bblock, &block);
    
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
#ifdef DEBUGPRINT
    std::cout << "Creating function: " << id.name << std::endl;
#endif
    return function;
}

Value * NIfStatement::codeGen(CodeGenContext& context) {
    this->generated = true;
    Value * exp = this->expression.codeGen(context);
    uint id = rand();
    std::stringstream trueName;
    trueName << "trueBlock." << id;
    std::stringstream falseName;
    falseName << "falseBlock." << id;
    std::stringstream endName;
    endName << "endBlock." << id;

    BasicBlock *trueBlock = BasicBlock::Create(ecfbContext, trueName.str(), context.currentBlock()->getParent());
    BasicBlock *falseBlock = BasicBlock::Create(ecfbContext, falseName.str(), context.currentBlock()->getParent());
    BasicBlock *endBlock = BasicBlock::Create(ecfbContext, endName.str(), context.currentBlock()->getParent());

    if (context.getCurrentEndBlock()) {
        BranchInst::Create(context.getCurrentEndBlock(), endBlock);
    }

    context.pushBlock(trueBlock, &this->block, endBlock);
    this->block.codeGen(context);
    context.popBlock();
    if (this->elseBlock) {
        context.pushBlock(falseBlock, this->elseBlock, endBlock);
        this->elseBlock->codeGen(context);
        context.popBlock();
    }
    
    BranchInst::Create(trueBlock, falseBlock, exp, context.currentBlock());

    auto containsIfOrWhile = [&](NStatement *state) {
        return dynamic_cast<NIfStatement *>(state) != nullptr ||
                dynamic_cast<NWhileStatement *>(state) != nullptr;
    };
    if (!this->block.contains(containsIfOrWhile)) {
        BranchInst::Create(endBlock, trueBlock);
    }
    if (this->elseBlock != nullptr &&
        !this->elseBlock->contains(containsIfOrWhile)) {
        BranchInst::Create(endBlock, falseBlock);
    } else if (this->elseBlock == nullptr) {
        BranchInst::Create(endBlock, falseBlock);
    }

    NBlock *nblock = &context.currentNBlock().copy();
    auto locals = context.locals();
    context.popBlock();
    context.pushBlock(endBlock, nblock);
    context.setCurrentLocals(locals);
#ifdef DEBUGPRINT
    std::cout << "Creating if statement with ID: " << id << std::endl;
#endif
    
    return endBlock;
}

Value * NWhileStatement::codeGen(CodeGenContext& context) {
    this->generated = true;
    Value * exp = this->expression.codeGen(context);
    uint id = rand();
    std::stringstream trueWhileName;
    trueWhileName << "trueWhileBlock." << id;
    std::stringstream falseWhileName;
    falseWhileName << "falseWhileBlock." << id;

    BasicBlock *trueWhileBlock = BasicBlock::Create(ecfbContext, trueWhileName.str(), context.currentBlock()->getParent());
    BasicBlock *falseWhileBlock = BasicBlock::Create(ecfbContext, falseWhileName.str(), context.currentBlock()->getParent());

    BranchInst::Create(trueWhileBlock, falseWhileBlock, exp, context.currentBlock());

    context.pushBlock(trueWhileBlock, &this->block);
    this->block.codeGen(context);
    exp = this->expression.codeGen(context);
    BranchInst::Create(trueWhileBlock, falseWhileBlock, exp, context.currentBlock());
    context.popBlock();

    auto currentEndBlock = context.getCurrentEndBlock();
    
    NBlock *nblock = &context.currentNBlock().copy();
    auto locals = context.locals();
    context.popBlock();
    context.pushBlock(falseWhileBlock, nblock);
    context.setCurrentLocals(locals);
    nblock->codeGen(context);
    if (currentEndBlock) {
        BranchInst::Create(currentEndBlock, falseWhileBlock);
    }
#ifdef DEBUGPRINT
    std::cout << "Creating if statement with ID: " << id << std::endl;
#endif
    return falseWhileBlock;
}
