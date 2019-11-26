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
#include <typeinfo>

extern NBlock* programBlock;

/* Validations */
bool Node::validate(std::string& error, NBlock& currentBlock) {
    error = std::string("validate hasn't been overrided");
    return false;
}

bool NInteger::validate(std::string& error, NBlock& currentBlock) {
    return true;
}

bool NDouble::validate(std::string& error, NBlock& currentBlock) {
    return true;
}

bool NBoolean::validate(std::string& error, NBlock& currentBlock) {
    if (stringValue.compare("true") != 0 && stringValue.compare("false")) {
        error = std::string("Boolean value only accepts true or false");
        return false;
    }
    return true;
}

bool NIdentifier::validate(std::string& error, NBlock& currentBlock) {
    return true;
}

bool NMethodCall::validate(std::string& error, NBlock& currentBlock) {
    for (ExpressionIterator it = arguments.begin(); it != arguments.end(); it++) {
        if (!(**it).validate(error, currentBlock)) {
            return false;
        }
    }
    return true;
}

bool NBinaryOperator::validate(std::string& error, NBlock& currentBlock) {
    switch (op) {
        case TPLUS:
        case TMINUS:
        case TMUL:
        case TDIV:
            if (lhs.resultType() == TBOOLEAN) {
                error = std::string("Left value has to return a number");
            } else if (rhs.resultType() == TBOOLEAN) {
                error = std::string("Right value has to return a number");
            } else return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            break;
        case TCEQ:
        case TCNE:
        case TCLT:
        case TCLE:
        case TCGT:
        case TCGE:
            if (lhs.resultType() != TBOOLEAN) {
                error = std::string("Left value has to return a boolean");
            } else if (rhs.resultType() != TBOOLEAN) {
                error = std::string("Right value has to return a boolean");
            } else return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            break;
    }
    return false;
}

bool NAssignment::validate(std::string& error, NBlock& currentBlock) {
    return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
}

bool NBlock::validate(std::string& error, NBlock& currentBlock) {
    for (StatementIterator it = statements.begin(); it != statements.end(); it++) {
        if (!(**it).validate(error, currentBlock)) {
            return false;
        }
    }
    return true;
}

bool NExpressionStatement::validate(std::string& error, NBlock& currentBlock) {
    return expression.validate(error, currentBlock);
}

bool NReturnStatement::validate(std::string& error, NBlock& currentBlock) {
    return expression.validate(error, currentBlock);
}

bool NVariableDeclaration::validate(std::string& error, NBlock& currentBlock) {
    if (assignmentExpr != NULL) {
        
    }
    return true;
}

bool NFunctionDeclaration::validate(std::string& error, NBlock& currentBlock) {
    
    return true;
}
/* Expression result type */
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

int NMethodCall::resultType() {
    return -1;
}
