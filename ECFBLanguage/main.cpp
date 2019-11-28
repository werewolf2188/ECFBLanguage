//
//  main.cpp
//  ECFBLanguage
//
//  Created by Enrique Ricalde on 11/19/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include <iostream>
#include <stdio.h>

#include "codegen.hpp"
#include "semantics.hpp"

extern NBlock* programBlock;
extern int yyparse();
extern void yyset_in (FILE *  in_str );

void createCoreFunctions(CodeGenContext& context);

int main(int argc, const char * argv[]) {
    // insert code here...
//    analyze_tokens();
    // This should be coming from the args.
    FILE *file = fopen("./examples/good_example.ec", "r");
    yyset_in(file);
//    std::cout << "Please add a line of code" << std::endl;
    
    yyparse();
    programBlock->printString(0);
    programBlock->separateVariablesAndFunctions();
    std::string error;
    
    if (programBlock->validate(error, *programBlock)) {
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();
        CodeGenContext context;
        createCoreFunctions(context);
        context.generateCode(*programBlock);
        context.runCode();
    } else { 
        std::cout << "There was a problem: " << error << std::endl;
    }
    
//    std::cout << programBlock << std::endl;
    return 0;
}
