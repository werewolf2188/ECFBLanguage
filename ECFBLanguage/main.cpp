//
//  main.cpp
//  ECFBLanguage
//
//  Created by Enrique Ricalde on 11/19/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include <iostream>
#include "test.h"

extern "C" int sum(int x, int y);

int main(int argc, const char * argv[]) {
    // insert code here...
    test();
    std::cout << "The number is " << sum(5, 6) << ".\n";
    return 0;
}
