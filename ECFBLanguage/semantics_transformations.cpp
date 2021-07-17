//
//  semantics_transformations.cpp
//  ECFBLanguage
//
//  Created by Enrique Ricalde on 11/27/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include "semantics_transformations.hpp"
#include "parser.hpp"
extern NBlock* programBlock;
extern NFunctionDeclaration * echod;
extern NFunctionDeclaration * echob;
extern NFunctionDeclaration * echoi;

void transformVariableDeclaration(NVariableDeclaration *vd, NBlock& block);

NExpression* transformVariableDeclaration(NExpression *previousExpression, int expectedType, NBlock& block);

NExpression* transformInt(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformFloat(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformMethod(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformIdentifier(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformBinaryExpression(NBinaryOperator * previousBinaryOperator, int expectedType, NBlock& block);
NMethodCall* transformMethodCallArguments(NMethodCall *methodCall, NBlock& block);


void transform(NBlock& block) {
    // We check the variables first
    VariableList variables = block.getVariables();
    for (VariableIterator it = variables.begin(); it != variables.end(); it++) {
        NVariableDeclaration *vd = (*it);
        transformVariableDeclaration(vd, block);
    }
    // Then we go to the functions as blocks
    StatementList functions = block.getFunctions();
    for(StatementIterator it = functions.begin(); it != functions.end(); it++) {
        NFunctionDeclaration *function = ((NFunctionDeclaration *)(*it));
        // Add variables from old block to new block
        for (VariableIterator it = variables.begin(); it != variables.end(); it++) {
            function->block.addVariable(*it);
        }
        transform(function->block);
    }
    
    // Then we check each of the assignments to see that everything's ok.
    for(StatementIterator it = block.statements.begin(); it != block.statements.end(); it++) {
        if (NExpressionStatement *exprSt = dynamic_cast<NExpressionStatement *>(*it)) {
            if (NAssignment *assign = dynamic_cast<NAssignment *>(&(exprSt->expression))) {
                NExpression * nRHS = transformVariableDeclaration(&assign->rhs, assign->lhs.resultType(block), block);
                NAssignment *nAssign = new NAssignment(assign->lhs, *nRHS);
                NExpressionStatement * nExpressionStatement = new NExpressionStatement(*nAssign);
                *it = nExpressionStatement;
            } else if(NMethodCall *methodCall = dynamic_cast<NMethodCall *>(&(exprSt->expression))) {
                methodCall = transformMethodCallArguments(methodCall, block);
                if (methodCall != NULL)
                    *it = new NExpressionStatement(*methodCall);
            }
        } else if (NIfStatement *ifState = dynamic_cast<NIfStatement *>(*it)) {
            NExpression * expr = transformVariableDeclaration(&ifState->expression, ifState->expression.resultType(block), block);
            // Add variables from old block to new block
            for (VariableIterator it = variables.begin(); it != variables.end(); it++) {
                ifState->block.addVariable(*it);
            }
            ifState->block.separateVariablesAndFunctions();
            transform(ifState->block);
            if (ifState->elseBlock != NULL) {
                // Add variables from old block to new block
                for (VariableIterator it = variables.begin(); it != variables.end(); it++) {
                    ifState->elseBlock->addVariable(*it);
                }
                ifState->elseBlock->separateVariablesAndFunctions();
                transform(*(ifState->elseBlock));
                *it = new NIfStatement(*expr, ifState->block, ifState->elseBlock);
            } else {
                // Add variables from old block to new block
                 *it = new NIfStatement(*expr, ifState->block);
            }
            
        } else if (NWhileStatement *whileState = dynamic_cast<NWhileStatement *>(*it)) {
            NExpression * expr = transformVariableDeclaration(&whileState->expression, whileState->expression.resultType(block), block);
            // Add variables from old block to new block
            transform(whileState->block);
            *it = new NWhileStatement(*expr, whileState->block);            
        }
    }
}
//
void transformVariableDeclaration(NVariableDeclaration *vd, NBlock& block) {
    int type = vd->type.resultType(block);
    if (vd->assignmentExpr != NULL) {
        vd->assignmentExpr = transformVariableDeclaration(vd->assignmentExpr, type, block);
    } else {
        StatementList statements = block.statements;
        for(StatementIterator it = statements.begin(); it != statements.end(); it++) {
            if (NAssignment *assign = dynamic_cast<NAssignment*>(*it)) {
                if (assign->lhs.name.compare(vd->id.name) == 0) {
                    assign->rhs = *transformVariableDeclaration(&assign->rhs, type, block);
                    break;
                }
            }
        }
    }
//    printf("Test");
    
}
NExpression* transformVariableDeclaration(NExpression *previousExpression, int expectedType, NBlock& block) {
    if (expectedType != TBOOLEAN && expectedType != TSTRING && expectedType != -1) {
        if (NBinaryOperator * binary = dynamic_cast<NBinaryOperator* >(previousExpression)) {
            return transformBinaryExpression(binary, expectedType, block);
        } else if (NIdentifier *id = dynamic_cast<NIdentifier*>(previousExpression)) {
            return transformIdentifier(id, expectedType, block);
        } else if (NInteger * intId = dynamic_cast<NInteger *>(previousExpression)) {
            return transformFloat(intId, expectedType, block);
        } else if (NDouble *doubleId = dynamic_cast<NDouble *>(previousExpression)) {
            return transformInt(doubleId, expectedType, block);
        } else if (NMethodCall *call = dynamic_cast<NMethodCall*>(previousExpression)) {
            return transformMethod(call, expectedType, block);
        }
    } else if (expectedType == TBOOLEAN) {
        std::string name = typeid((*previousExpression)).name();
        
        if (NBinaryOperator * oper = dynamic_cast<NBinaryOperator* >(previousExpression)) {
            int newExpectedType = oper->lhs.resultType(block);
            NExpression * lhs = (transformVariableDeclaration(&oper->lhs, newExpectedType, block));
            NExpression * rhs = (transformVariableDeclaration(&oper->rhs, newExpectedType, block));
            return new NBinaryOperator(*lhs, oper->op, *rhs);
        }
    }
    return previousExpression;
}

NExpression* transformInt(NExpression * previousValue, int expectedType, NBlock& block) {
    if (previousValue->resultType(block) != expectedType) {
        NIdentifier *id = new NIdentifier(std::string("int"));
        return new NDataConversion(*id, *previousValue);
    }
    return previousValue;
}

NExpression* transformBinaryExpression(NBinaryOperator * previousBinaryOperator, int expectedType, NBlock& block) {
    int type = previousBinaryOperator->resultType(block);
    NBinaryOperator * newBinary = new NBinaryOperator(
                                                      *transformVariableDeclaration(&previousBinaryOperator->lhs, expectedType, block),
                                                      previousBinaryOperator->op,
                                                      *transformVariableDeclaration(&previousBinaryOperator->rhs, expectedType, block));
    return transformIdentifier(newBinary, expectedType, block);
}

NExpression* transformFloat(NExpression * previousValue, int expectedType, NBlock& block) {
    if (previousValue->resultType(block) != expectedType) {
        NIdentifier *id = new NIdentifier(std::string("double"));
        return new NDataConversion(*id, *previousValue);
    }
    return previousValue;
}

NExpression* transformIdentifier(NExpression * previousValue, int expectedType, NBlock& block) {
    if (previousValue->resultType(block) != expectedType) {
        
        NIdentifier *id = new NIdentifier(std::string(expectedType == TDOUBLE ? "double" : "int"));
        return new NDataConversion(*id, *previousValue);
    }
    return previousValue;
}

NExpression* transformMethod(NExpression * previousValue, int expectedType, NBlock& block) {
    if (previousValue->resultType(block) != expectedType) {
           
           NIdentifier *id = new NIdentifier(std::string(expectedType == TDOUBLE ? "double" : "int"));
           return new NDataConversion(*id, *(transformMethodCallArguments((NMethodCall*)previousValue, block)));
       }
       return transformMethodCallArguments((NMethodCall*)previousValue, block);
}


NMethodCall* transformMethodCallArguments(NMethodCall *methodCall, NBlock& block) {
    bool exists = false;
    NFunctionDeclaration *fRef = NULL;
    
    for (StatementIterator it = programBlock->getFunctions().begin(); it != programBlock->getFunctions().end(); it++) {
        std::string fName = ((NFunctionDeclaration *)(*it))->id.name;
        if (fName.find(methodCall->id.name) != std::string::npos) {
            exists = true;
            fRef = (NFunctionDeclaration *)(*it);
            break;
        }
    }
    if (methodCall->id.name.find(echod->id.name) != std::string::npos) {
        fRef = echod;
        exists = true;
    } else if (methodCall->id.name.find(echob->id.name) != std::string::npos) {
        fRef = echob;
        exists = true;
    } else if (methodCall->id.name.find(echoi->id.name) != std::string::npos) {
        fRef = echoi;
        exists = true;
    } else if (methodCall->id.name.find(block.printf) != std::string::npos) {
        return methodCall;
    } else if (methodCall->id.name.find(block.gets) != std::string::npos) {
        return methodCall;
    }
    if (exists) {
        //Arguments size have to be the same
        if (fRef->arguments.size() != methodCall->arguments.size()) {
            return NULL;
        }
        if (fRef->arguments.size() > 0 && methodCall->arguments.size() > 0) {
            //Find arguments result types are correct
            VariableIterator it1 = fRef->arguments.begin();
            ExpressionIterator it2 = methodCall->arguments.begin();
            do {
                
                int type1 = (**it1).type.resultType(*programBlock);
                int type2 = (**it2).resultType(*programBlock);
                if (type1 != type2) {
                    if ((type1 == TDOUBLE && type2 == TINTEGER) || (type1 == TINTEGER && type2 == TDOUBLE)) {
                        *it2 = transformVariableDeclaration(*it2, type1, block);
                    }
                }
                if (NMethodCall *call = dynamic_cast<NMethodCall*>(*it2)) {
                   *it2 = transformMethodCallArguments(call, block);
                }
                it1++;
                it2++;
            } while(it1 != fRef->arguments.end() && it2 != methodCall->arguments.end());
            return methodCall;
        }
    }
    return NULL;
}
