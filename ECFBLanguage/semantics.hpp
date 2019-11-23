//
//  semantics.h
//  ECFBLanguage
//
//  Created by Enrique on 11/21/19.
//  Copyright © 2019 american airlines. All rights reserved.
//

#ifndef semantics_h
#define semantics_h
#include <iostream>
#include <vector>
#include <string>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

typedef std::vector<NStatement*>::iterator StatementIterator;
typedef std::vector<NExpression*>::iterator ExpressionIterator;
typedef std::vector<NVariableDeclaration*>::iterator VariableIterator;

class Node {
public:
    virtual ~Node() { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void printString(int spaces) { }
};

class NExpression : public Node {
    
};

class NStatement : public Node {
    
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Integer Expression: " << value << std::endl;
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Double Expression: " << value << std::endl;
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string name) : name(name) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Identifier Expression: " << name << std::endl;
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
public:
    NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(NIdentifier& id, ExpressionList& arguments) : id(id), arguments(arguments) { }
    NMethodCall(NIdentifier& id) : id(id) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Method Call Expression: " << std::endl;
        
        this->id.printString(spaces + 1);
        
        for(ExpressionIterator it = this->arguments.begin(); it != this->arguments.end(); ++it) {
            (*it)->printString(spaces + 1);
        }
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) : lhs(lhs), op(op), rhs(rhs) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Binary operator Expression: " << std::endl;
        
        this->lhs.printString(spaces + 1);
        this->rhs.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : lhs(lhs), rhs(rhs) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Assignment Expression: " << std::endl;
        
        this->lhs.printString(spaces + 1);
        this->rhs.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Block Expression: " << std::endl;
        
        for(StatementIterator it = this->statements.begin(); it != this->statements.end(); ++it) {
            (*it)->printString(spaces + 1);
        }
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : expression(expression) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Expression Statement: " << std::endl;
        
        this->expression.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReturnStatement: public NStatement {
public:
    NExpression& expression;
    NReturnStatement(NExpression& expression) :
        expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
    NIdentifier& type;
    NIdentifier& id;
    NExpression *assignmentExpr = NULL;
    NVariableDeclaration(NIdentifier& type, NIdentifier& id) :
    type(type), id(id) { }
    NVariableDeclaration(NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
        type(type), id(id), assignmentExpr(assignmentExpr) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Variable Declaration: " << std::endl;
        
        this->type.printString(spaces + 1);
        this->id.printString(spaces + 1);
        if (this->assignmentExpr != NULL) {
            this->assignmentExpr->printString(spaces + 1);
        }
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
    NIdentifier& type;
    NIdentifier& id;
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(NIdentifier& type, NIdentifier& id,
            VariableList& arguments, NBlock& block) :
        type(type), id(id), arguments(arguments), block(block) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Function Declaration: " << std::endl;
        
        this->type.printString(spaces + 1);
        this->id.printString(spaces + 1);
        for (VariableIterator it = this->arguments.begin(); it != this->arguments.end(); ++it) {
            (*it)->printString(spaces + 1);
        }
        this->block.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};
#endif /* semantics_h */
