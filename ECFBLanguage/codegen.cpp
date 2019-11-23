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
    
}

/* Executes the AST by running the main function */
//GenericValue CodeGenContext::runCode() {
//    
//}

/* -- Code Generation -- */

Value * NInteger::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NDouble::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NIdentifier::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NMethodCall::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NBinaryOperator::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NAssignment::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NBlock::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NExpressionStatement::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NVariableDeclaration::codeGen(CodeGenContext& context) {
    return NULL;
}

Value * NFunctionDeclaration::codeGen(CodeGenContext& context) {
    return NULL;
}
