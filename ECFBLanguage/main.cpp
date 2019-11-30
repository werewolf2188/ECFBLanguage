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

typedef struct Options {
    const char* file = NULL;
} Options;

Options getOptions(int argc, const char * argv[]);

int main(int argc, const char * argv[]) {
    // insert code here...
//    analyze_tokens();
    
    Options options = getOptions(argc, argv);
    if (options.file == NULL) {
        std::cout << "Please specify to compile" << std::endl;
        return 9;
    }
    FILE *file = fopen(options.file, "r");
    if (file == NULL) {
        std::cout << "File does not exists" << std::endl;
        return 9;
    }
    yyset_in(file);
    
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
    return 0;
}


Options getOptions(int argc, const char * argv[]) {
    Options options;
    if (argc > 1) {
        options.file = argv[1];
    }
    return options;
}
