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
        transform(function->block);
    }
    
    // Then we check each of the assignments to see that everything's ok.
    for(StatementIterator it = block.statements.begin(); it != block.statements.end(); it++) {
        std::string name = typeid((**it)).name();
        if (name.find("NExpressionStatement") != std::string::npos) {
            NExpressionStatement *exprSt = ((NExpressionStatement*)(*it));
            std::string name = typeid((*exprSt).expression).name();
            if (name.find("NAssignment") != std::string::npos) {
                NAssignment *assign = ((NAssignment*)&((*exprSt).expression));
                NExpression * nRHS = transformVariableDeclaration(&assign->rhs, assign->lhs.resultType(block), block);
                NAssignment *nAssign = new NAssignment(assign->lhs, *nRHS);
                NExpressionStatement * nExpressionStatement = new NExpressionStatement(*nAssign);
                *it = nExpressionStatement;
            } else if(name.find("NMethodCall") != std::string::npos) {
                NMethodCall *methodCall = ((NMethodCall*)&((*exprSt).expression));
                methodCall = transformMethodCallArguments(methodCall, block);
                if (methodCall != NULL)
                    *it = new NExpressionStatement(*methodCall);
            }
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
            std::string name = typeid((**it)).name();
            if (name.find("NAssignment") != std::string::npos) {
                NAssignment *assign = ((NAssignment*)(*it));
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
        std::string name = typeid((*previousExpression)).name();
        
        if (name.find("NBinaryOperator") != std::string::npos) {
            return transformBinaryExpression((NBinaryOperator *)previousExpression, expectedType, block);
        } else if (name.find("NIdentifier") != std::string::npos) {
            return transformIdentifier(previousExpression, expectedType, block);
        } else if (name.find("NInteger") != std::string::npos) {
            return transformFloat(previousExpression, expectedType, block);
        } else if (name.find("NDouble") != std::string::npos) {
            return transformInt(previousExpression, expectedType, block);
        } else if (name.find("NMethodCall") != std::string::npos) {
            return transformMethod(previousExpression, expectedType, block);
        }
    } else if (expectedType == TBOOLEAN) {
        std::string name = typeid((*previousExpression)).name();
        
        if (name.find("NBinaryOperator") != std::string::npos) {
            NBinaryOperator * oper = (NBinaryOperator *)previousExpression;
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
                std::string name = typeid(**it2).name();
                if (name.find("NMethodCall") != std::string::npos) {
                   *it2 = transformMethodCallArguments((NMethodCall*)(*it2), block);
                }
                it1++;
                it2++;
            } while(it1 != fRef->arguments.end() && it2 != methodCall->arguments.end());
            return methodCall;
        }
    }
    return NULL;
}
