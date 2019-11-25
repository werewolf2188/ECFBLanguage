//
//  semantics_validations.cpp
//  ECFBLanguage
//
//  Created by Enrique Ricalde on 11/25/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include <stdio.h>
#include "semantics.hpp"
#include "parser.hpp"

int NExpression::resultType() {
    return -1;
}

int NInteger::resultType() {
    return TINTEGER;
}

int NDouble::resultType() {
    return TDOUBLE;
}

int NBoolean::resultType() {
    return TBOOLEAN;
}
 
int NBinaryOperator::resultType() {
    switch (op) {
        case TPLUS: return biggerType();
        case TMINUS: return biggerType();
        case TMUL: return biggerType();
        case TDIV: return biggerType();
            /* TODO comparison */
        case TCEQ: return TBOOLEAN;
        case TCNE: return TBOOLEAN;
        case TCLT: return TBOOLEAN;
        case TCLE: return TBOOLEAN;
        case TCGT: return TBOOLEAN;
        case TCGE: return TBOOLEAN;
    }
    return -1;
}

int NBinaryOperator::biggerType() {
    if (lhs.resultType() == TDOUBLE || rhs.resultType() == TDOUBLE) return TDOUBLE;
    
    return TINTEGER;
}
