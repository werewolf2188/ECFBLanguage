//
//  main.cpp
//  ECFBLanguage
//
//  Created by Enrique Ricalde on 11/19/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include <iostream>
#include "semantics.hpp"

extern NBlock* programBlock;
extern int yyparse();

//extern "C" int sum(int x, int y);

int main(int argc, const char * argv[]) {
    // insert code here...
//    analyze_tokens();
    std::cout << "Please add a line of code" << std::endl;
    yyparse();
    std::cout << programBlock << std::endl;
//    std::cout << "The number is " << sum(5, 6) << ".\n";
    return 0;
}
