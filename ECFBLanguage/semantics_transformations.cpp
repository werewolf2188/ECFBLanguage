//
//  semantics_transformations.cpp
//  ECFBLanguage
//
//  Created by Enrique Ricalde on 11/27/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#include "semantics_transformations.hpp"
#include "parser.hpp"
void transformVariableDeclaration(NVariableDeclaration *vd, NBlock& block);

NExpression* transformVariableDeclaration(NExpression *previousExpression, int expectedType, NBlock& block);

NExpression* transformInt(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformFloat(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformMethod(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformIdentifier(NExpression * previousValue, int expectedType, NBlock& block);
NExpression* transformBinaryExpression(NBinaryOperator * previousBinaryOperator, int expectedType, NBlock& block);

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
        if (name.find("NAssignment") != std::string::npos) {
            NAssignment *assign = ((NAssignment*)(*it));
            assign->rhs = *transformVariableDeclaration(&assign->rhs, assign->resultType(block), block);
        } else if (name.find("NExpressionStatement") != std::string::npos) {
            NExpressionStatement *exprSt = ((NExpressionStatement*)(*it));
            std::string name = typeid((*exprSt).expression).name();
            if (name.find("NAssignment") != std::string::npos) {
                NAssignment *assign = ((NAssignment*)&((*exprSt).expression));
                NExpression * nRHS = transformVariableDeclaration(&assign->rhs, assign->lhs.resultType(block), block);
                NAssignment *nAssign = new NAssignment(assign->lhs, *nRHS);
                NExpressionStatement * nExpressionStatement = new NExpressionStatement(*nAssign);
                *it = nExpressionStatement;
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
    // TODO: Balance binary operators inside boolean assignments
    if (expectedType != TBOOLEAN && expectedType != -1) {
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
    previousBinaryOperator->lhs = *transformVariableDeclaration(&previousBinaryOperator->lhs, expectedType, block);
    previousBinaryOperator->rhs = *transformVariableDeclaration(&previousBinaryOperator->rhs, expectedType, block);
    return transformIdentifier(previousBinaryOperator, expectedType, block);
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
           return new NDataConversion(*id, *previousValue);
       }
       return previousValue;
}
