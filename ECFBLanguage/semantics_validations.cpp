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

bool NIdentifier::isType() {
    return name.compare("int") == 0 || name.compare("double") == 0 || name.compare("void") == 0 || name.compare("boolean") == 0;
}

bool NIdentifier::validate(std::string& error, NBlock& currentBlock) {
    // If the identifier is a type
    if (this->isType()) {
        return true;
    }
    
    bool exists = false;
    VariableList vars = currentBlock.getVariables();
    VariableList globalVars = programBlock->getVariables();
    StatementList functions = programBlock->getFunctions();
    
    // If the identifier is inside the block
    for (VariableIterator it = vars.begin(); it != vars.end(); it++) {
        
        std::string vName = (**it).id.name;
        if (vName.find(name) != std::string::npos) {
            exists = true;
        }
    }
    // If the identifier is inside the main block
    for (VariableIterator it = globalVars.begin(); it != globalVars.end(); it++) {

        std::string vName = (**it).id.name;
        if (vName.find(name) != std::string::npos) {
            exists = true;
        }
    }
    // If the identifier is inside the main block and its a function
    for (StatementIterator it = functions.begin(); it != functions.end(); it++) {

        std::string vName = ((NFunctionDeclaration *)(*it))->id.name;
        if (vName.find(name) != std::string::npos) {
            exists = true;
        }
    }
    if (!exists) {
        error = std::string("Variable doesn't exists");
    }
    return exists;
}

bool NMethodCall::validate(std::string& error, NBlock& currentBlock) {
    
    if (id.isType()) {
        error = std::string("Function call cannot have the name of a type");
        return false;
    }
    
    bool exists = false;
    NFunctionDeclaration *fRef = NULL;
    // Find declaration
    for (StatementIterator it = currentBlock.getFunctions().begin(); it != currentBlock.getFunctions().end(); it++) {
        std::string fName = ((NFunctionDeclaration *)(*it))->id.name;
        if (fName.find(this->id.name) != std::string::npos) {
            exists = true;
            fRef = (NFunctionDeclaration *)(*it);
            break;
        }
    }
    
    for (StatementIterator it = programBlock->getFunctions().begin(); it != programBlock->getFunctions().end(); it++) {
        std::string fName = ((NFunctionDeclaration *)(*it))->id.name;
        if (fName.find(this->id.name) != std::string::npos) {
            exists = true;
            fRef = (NFunctionDeclaration *)(*it);
            break;
        }
    }
    
    // We don't need to check the arguments for echod nor echoi
    if (this->id.name.find(currentBlock.echod) != std::string::npos) {
        for (ExpressionIterator it = arguments.begin(); it != arguments.end(); it++) {
            if (!(**it).validate(error, currentBlock)) {
                return false;
            }
        }
        return true;
    }
    if (this->id.name.find(currentBlock.echoi) != std::string::npos) {
        for (ExpressionIterator it = arguments.begin(); it != arguments.end(); it++) {
            if (!(**it).validate(error, currentBlock)) {
                return false;
            }
        }
        return true;
    }
    
    if (!exists) {
        error = std::string("Method doesnt exists");
        return false;
    }
    
    //Arguments size have to be the same
    if (fRef->arguments.size() != this->arguments.size()) {
        error = std::string("Arguments size is not the same with its declaration");
        return false;
    }
    if (fRef->arguments.size() > 0 && this->arguments.size() > 0) {
        //Find arguments result types are correct
        VariableIterator it1 = fRef->arguments.begin();
        ExpressionIterator it2 = this->arguments.begin();
        do {
            
            int type1 = (**it1).type.resultType(currentBlock);
            int type2 = (**it2).resultType(currentBlock);
            if (type1 != type2) {
                error = std::string("Arguments type is not the same with its declaration");
                return false;
            }
            it1++;
            it2++;
        } while(it1 != fRef->arguments.end() && it2 != this->arguments.end());
    }
    
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
        case TREMAIN:
            if (lhs.resultType(currentBlock) == TBOOLEAN) {
                error = std::string("Left value has to return a number");
            } else if (rhs.resultType(currentBlock) == TBOOLEAN) {
                error = std::string("Right value has to return a number");
            } else return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            break;
        case TCEQ:
        case TCNE:
        case TCLT:
        case TCLE:
        case TCGT:
        case TCGE:
            
            bool lhsIsNumber = std::string(typeid(lhs).name()).find("Integer") != std::string::npos || std::string(typeid(lhs).name()).find("Double") != std::string::npos;
            bool rhsIsNumber = std::string(typeid(rhs).name()).find("Integer") != std::string::npos || std::string(typeid(rhs).name()).find("Double") != std::string::npos;
            
            if ((lhs.resultType(currentBlock) == TBOOLEAN && rhs.resultType(currentBlock) == TBOOLEAN)
                || (lhsIsNumber && rhsIsNumber)) {
                return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            }
            else if (lhs.resultType(currentBlock) != TBOOLEAN ) {
                error = std::string("Left value has to return a boolean");
            } else if (rhs.resultType(currentBlock) != TBOOLEAN) {
                error = std::string("Right value has to return a boolean");
            }
            break;
    }
    return false;
}

bool NUnaryOperator::validate(std::string &error, NBlock &currentBlock) {
    switch (op) {
        case TMINUS:
            if (rhs.resultType(currentBlock) == TBOOLEAN) {
                error = std::string("Right value has to return a number");
            } else return rhs.validate(error, currentBlock);
            break;
        case TNOT:
            if (rhs.resultType(currentBlock) != TBOOLEAN) {
                error = std::string("Right value has to return a boolean");
            } else return rhs.validate(error, currentBlock);
            break;
    }
    return false;
}

bool NAssignment::validate(std::string& error, NBlock& currentBlock) {
    NVariableDeclaration *variable = NULL;
    for (VariableIterator it = currentBlock.getVariables().begin(); it != currentBlock.getVariables().end(); it++) {
        std::string vName = (**it).id.name;
        if (vName.find(lhs.name) != std::string::npos) {
            variable = *it;
        }
    }
    // Variable does not exists
    if (variable == NULL) {
        error = std::string("Not existing variable cannot be assigned");
        return false;
    } else {
        int lType = variable->type.resultType(currentBlock);
        int rType = rhs.resultType(currentBlock);
        
        if (lhs.isType()) {
            error = std::string("Function cannot have the name of a type");
            return false;
        }
        
        if (lType == rType
//            || ((lType == TDOUBLE || lType == TINTEGER) && (rType == TDOUBLE || rType == TINTEGER))
            ) {
            return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
        } else {
            error = std::string("Cannot assign a different type to a variable");
            return false;
        }
    }
}

bool NBlock::validate(std::string& error, NBlock& currentBlock) {
    for (StatementIterator it = statements.begin(); it != statements.end(); it++) {
        if (!(**it).validate(error, *this)) {
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
    if (!type.isType()) {
        error = std::string("Variable type is not a type");
        return false;
    }
    
    if (id.isType()) {
        error = std::string("Variable cannot have the name of a type");
        return false;
    }
    
    if (assignmentExpr != NULL) {
        int lType = type.resultType(currentBlock);
        int rType = assignmentExpr->resultType(currentBlock);
        if (lType == rType
//            || ((lType == TDOUBLE || lType == TINTEGER) && (rType == TDOUBLE || rType == TINTEGER))
            ) {
            return assignmentExpr->validate(error, currentBlock);
            
        } else {
            error = std::string("Assignment type incorrect for variable");
            return false;
        }
        
    }
    
    
    return true;
}

bool NFunctionDeclaration::validate(std::string& error, NBlock& currentBlock) {
    // We check there are no function declarations inside the function
    if (this->block.getFunctions().size() > 0) {
        error = std::string("No functions can be declared inside a function");
        return false;
    }
    // Arguments are not supposed to have an assignment expression
    bool variables = true;
    for (VariableIterator it = arguments.begin(); it != arguments.end(); it++) {
        if ((*it)->assignmentExpr != NULL) {
            error = std::string("No functions arguments can have assignment expressions");
            return false;
        }
        variables = variables && (*it)->validate(error, currentBlock);
    }
    
    if (type.resultType(currentBlock) != -1) {
        // There cannot be more than one return
        if (noOfReturns > 1) {
            error = std::string("No functions can have more than one return");
            return false;
        }
        // if there is no return but it asks for a return type, then is an error
        if (returnStatement == NULL) {
            error = std::string("Function is asking for a return type");
            return false;
        }
        // If the result
        else if (returnStatement != NULL && returnStatement->expression.resultType(block) != type.resultType(currentBlock)) {
            error = std::string("Return type is different from what function is expecting");
            return false;
        }
    }
    
    if (!type.isType()) {
        error = std::string("Return type is not a type");
        return false;
    }
    
    if (id.isType()) {
        error = std::string("Function cannot have the name of a type");
        return false;
    }
    
    return type.validate(error, currentBlock) && id.validate(error, currentBlock) && variables && block.validate(error, currentBlock);
}

/* Expression result type */
int NExpression::resultType(NBlock& currentBlock) {
    return -1;
}

int NInteger::resultType(NBlock& currentBlock) {
    return TINTEGER;
}

int NDouble::resultType(NBlock& currentBlock) {
    return TDOUBLE;
}

int NBoolean::resultType(NBlock& currentBlock) {
    return TBOOLEAN;
}
 
int NIdentifier::resultType(NBlock& currentBlock) {
    if (name.compare("int") == 0) {
        return TINTEGER;
    } else if (name.compare("double") == 0) {
        return TDOUBLE;
    } else if (name.compare("boolean") == 0) {
        return TBOOLEAN;
    } else {
        for (VariableIterator it = currentBlock.getVariables().begin(); it != currentBlock.getVariables().end(); it++) {
            std::string vName = (**it).id.name;
            if (vName.compare(name) == 0) {
                return (*it)->type.resultType(currentBlock);
            }
        }
    }
    return -1;
}
int NBinaryOperator::resultType(NBlock& currentBlock) {
    if (lhs.resultType(currentBlock) == TDOUBLE || rhs.resultType(currentBlock) == TDOUBLE) resultingType = TDOUBLE;
    else resultingType = TINTEGER;
    
    switch (op) {
        case TPLUS: return resultingType;
        case TMINUS: return resultingType;
        case TMUL: return resultingType;
        case TDIV: return resultingType;
        case TREMAIN: return resultingType;
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

int NUnaryOperator::resultType(NBlock& currentBlock) {
    if (rhs.resultType(currentBlock) == TDOUBLE) resultingType = TDOUBLE;
    else resultingType = TINTEGER;
    
    switch (op) {
        case TMINUS: return resultingType;
        case TNOT: return TBOOLEAN;
    }
    return -1;
}

int NMethodCall::resultType(NBlock& currentBlock) {
    NFunctionDeclaration *fRef = NULL;
    // Find declaration
    for (StatementIterator it = currentBlock.getFunctions().begin(); it != currentBlock.getFunctions().end(); it++) {
        std::string fName = ((NFunctionDeclaration *)(*it))->id.name;
        if (fName.find(this->id.name) != std::string::npos) {
            
            fRef = (NFunctionDeclaration *)(*it);
            break;
        }
    }
    
    for (StatementIterator it = programBlock->getFunctions().begin(); it != programBlock->getFunctions().end(); it++) {
        std::string fName = ((NFunctionDeclaration *)(*it))->id.name;
        if (fName.find(this->id.name) != std::string::npos) {
            
            fRef = (NFunctionDeclaration *)(*it);
            break;
        }
    }
    
    return fRef != NULL ? fRef->type.resultType(*programBlock) : -1;
}
