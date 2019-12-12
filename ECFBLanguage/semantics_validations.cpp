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
#include <stdarg.h>
extern NBlock* programBlock;
extern NFunctionDeclaration * echod;
extern NFunctionDeclaration * echob;
extern NFunctionDeclaration * echoi;

NVariableDeclaration* createArgument(const char* type, const char* name) {
    NIdentifier* typeId = new NIdentifier(std::string(type));
    NIdentifier* nameId = new NIdentifier(std::string(name));
    return new NVariableDeclaration(*typeId, *nameId);
}

NFunctionDeclaration* createEmptyNFunctionDeclaration(const char* type, const char* name, int numArgs, ...) {
    NIdentifier* typeId = new NIdentifier(std::string(type));
    NIdentifier* nameId = new NIdentifier(std::string(name));
    VariableList arguments;
    va_list valist;
    /* initialize valist for num number of arguments */
    va_start(valist, numArgs);
    /* access all the arguments assigned to valist */
    for (int i = 0; i < numArgs; i++) {
        arguments.push_back(va_arg(valist, NVariableDeclaration*));
    }
    /* clean memory reserved for valist */
    va_end(valist);
    
    NBlock * block = new NBlock();
    NFunctionDeclaration *function = new NFunctionDeclaration(*typeId, *nameId, arguments, *block);
    return function;
}

NFunctionDeclaration * echod = createEmptyNFunctionDeclaration("void", "echod", 1, createArgument("double", "toPrint"));
NFunctionDeclaration * echob = createEmptyNFunctionDeclaration("void", "echob", 1, createArgument("boolean", "toPrint"));
NFunctionDeclaration * echoi = createEmptyNFunctionDeclaration("void", "echoi", 1, createArgument("int", "toPrint"));

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

bool NString::validate(std::string &error, NBlock &currentBlock) {
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
    return name.compare("int") == 0 || name.compare("double") == 0 || name.compare("void") == 0 || name.compare("boolean") == 0  || name.compare("string") == 0;
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
    
    // We don't need to check the arguments for echod, echoi nor echob
    if (this->id.name.find(echod->id.name) != std::string::npos
        || this->id.name.find(echob->id.name) != std::string::npos
        || this->id.name.find(echoi->id.name) != std::string::npos
        || this->id.name.find(currentBlock.printf) != std::string::npos) {
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
    
    if (this->lhs.resultType(currentBlock) == TSTRING || this->rhs.resultType(currentBlock) == TSTRING) {
        error = std::string("Binary operand cannot be applied to string");
        return false;
    }
    
    switch (op) {
        case TPLUS:
        case TMINUS:
        case TMUL:
        case TDIV:
        case TREMAIN:
            if (lhs.resultType(currentBlock) != TDOUBLE && lhs.resultType(currentBlock) != TINTEGER) {
                error = std::string("Left value has to return a number");
            } else if (rhs.resultType(currentBlock) != TDOUBLE && rhs.resultType(currentBlock) != TINTEGER) {
                error = std::string("Right value has to return a number");
            } else return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            break;
        case TCEQ:
        case TCNE:
            if ((lhs.resultType(currentBlock) != TSTRING  && lhs.resultType(currentBlock) != -1
                 && rhs.resultType(currentBlock) != TSTRING && rhs.resultType(currentBlock) != -1)) {
                return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            }
            else if (lhs.resultType(currentBlock) == TSTRING || lhs.resultType(currentBlock) == -1) {
                error = std::string("Left value has to return a number or a boolean");
            } else if (rhs.resultType(currentBlock) == TSTRING || rhs.resultType(currentBlock) == -1) {
                error = std::string("Right value has to return a number or a boolean");
            }
            break;
        case TCLT:
        case TCLE:
        case TCGT:
        case TCGE:
            if ((lhs.resultType(currentBlock) == TDOUBLE || lhs.resultType(currentBlock) == TINTEGER)
                && (rhs.resultType(currentBlock) == TDOUBLE || rhs.resultType(currentBlock) == TINTEGER)) {
                return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            }
            else if (lhs.resultType(currentBlock) != TDOUBLE && lhs.resultType(currentBlock) != TINTEGER) {
                error = std::string("Left value has to return a number");
            } else if (rhs.resultType(currentBlock) != TDOUBLE && rhs.resultType(currentBlock) != TINTEGER) {
                error = std::string("Right value has to return a number");
            }
            break;
        case TAND:
        case TOR:
            if ((lhs.resultType(currentBlock) == TBOOLEAN)
                && (rhs.resultType(currentBlock) == TBOOLEAN)) {
                return lhs.validate(error, currentBlock) && rhs.validate(error, currentBlock);
            }
            else if (lhs.resultType(currentBlock) != TBOOLEAN) {
                error = std::string("Left value has to return a boolean");
            } else if (rhs.resultType(currentBlock) != TBOOLEAN) {
                error = std::string("Right value has to return a boolean");
            }
            break;
            break;
    }
    return false;
}

bool NUnaryOperator::validate(std::string &error, NBlock &currentBlock) {
    
    if (this->rhs.resultType(currentBlock) == TSTRING) {
        error = std::string("Binary operand cannot be applied to string");
        return false;
    }
    
    this->resultType(currentBlock);
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

bool NDataConversion::validate(std::string &error, NBlock &currentBlock) {
    if (!this->type.isType()) {
        error = std::string("Type casting not using a type");
        return false;
    } else if (this->type.resultType(currentBlock) == TBOOLEAN || this->type.resultType(currentBlock) == TSTRING || this->type.resultType(currentBlock) == -1
               || (this->rhs.resultType(currentBlock) != TDOUBLE && this->rhs.resultType(currentBlock) != TINTEGER)) {
        error = std::string("Type casting cannot happen with non number types");
        return false;
    }
    previousType = rhs.resultType(currentBlock);
    resultingType = type.resultType(currentBlock);
    return rhs.validate(error, currentBlock);
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
    
    int times = 0;
    VariableList variables = currentBlock.getVariables();
    for (VariableIterator it = variables.begin(); it != variables.end(); it++) {
        std::string fName = ((NVariableDeclaration*)(*it))->id.name;
        if (fName.compare(this->id.name) == 0) {
            times++;
        }
    }
    
    if (times > 1) {
        error = std::string("Variable cannot be redeclared");
        return false;
    }
    
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
    
    if (this->id.name.find(echod->id.name) != std::string::npos
    || this->id.name.find(echob->id.name) != std::string::npos
    || this->id.name.find(echoi->id.name) != std::string::npos
    || this->id.name.find(currentBlock.printf) != std::string::npos
    || this->id.name.find(std::string("main")) != std::string::npos) {
        error = std::string("Cannot overwrite function");
        return false;
    }
    
    int times = 0;
    StatementList functions = currentBlock.getFunctions();
    for (StatementIterator it = functions.begin(); it != functions.end(); it++) {
        std::string fName = ((NFunctionDeclaration*)(*it))->id.name;
        if (fName.compare(this->id.name) == 0) {
            times++;
        }
    }
    
    if (times > 1) {
        error = std::string("Function cannot be redeclared");
        return false;
    }
    
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

int NString::resultType(NBlock &currentBlock) {
    return TSTRING;
}
 
int NIdentifier::resultType(NBlock& currentBlock) {
    if (name.compare("int") == 0) {
        return TINTEGER;
    } else if (name.compare("double") == 0) {
        return TDOUBLE;
    } else if (name.compare("boolean") == 0) {
        return TBOOLEAN;
    } else if (name.compare("string") == 0) {
        return TSTRING;
    }else {
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
        case TAND: return TBOOLEAN;
        case TOR: return TBOOLEAN;
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

int NDataConversion::resultType(NBlock &currentBlock) {
    return type.resultType(currentBlock);
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
