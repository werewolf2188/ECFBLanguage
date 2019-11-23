//
//  main.cpp
//  ECFBLanguage
//
//  Created by Enrique Ricalde on 11/19/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include <iostream>
#include "codegen.hpp"
#include "semantics.hpp"

extern NBlock* programBlock;
extern int yyparse();

//extern "C" int sum(int x, int y);

int main(int argc, const char * argv[]) {
    // insert code here...
//    analyze_tokens();
    std::cout << "Please add a line of code" << std::endl;
    yyparse();
    programBlock->printString(0);
    
    CodeGenContext context;
    context.generateCode(*programBlock);
    /* For now we will not run the code, until we can find the issue. */
    context.runCode();
//    std::cout << programBlock << std::endl;
//    std::cout << "The number is " << sum(5, 6) << ".\n";
    return 0;
}
