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
#include <functional>

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

class NBlock;

class Node {
protected:
    bool generated = false;
public:
    virtual ~Node() { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual void printString(int spaces) { }
    virtual bool validate(std::string& error, NBlock& currentBlock);
    inline bool wasGenerated() { return generated; }
};

class NExpression : public Node {
public:
    virtual int resultType(NBlock& currentBlock);
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
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Double Expression: " << value << std::endl;
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NBoolean : public NExpression {
protected:
    std::string  stringValue;
public:
    bool value;
    NBoolean(std::string * value) : value(value->compare("true") == 0) {
        stringValue = *value;
    }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Boolean Expression: " << value << std::endl;
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NString : public NExpression {
public:
    std::string * value;
    NString(std::string * value) {
        value->erase(std::remove(value->begin(), value->end(), '\"'), value->end());
        value->append("\n");
        this->value = value;
    }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "String Expression: " << *value << std::endl;
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string name) : name(name) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Identifier Expression: " << name << std::endl;
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
    virtual bool isType();
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
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NBinaryOperator : public NExpression {
protected:
    int resultingType;
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
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NUnaryOperator : public NExpression {
protected:
    int resultingType;
public:
    int op;
    
    NExpression& rhs;
    NUnaryOperator(int op, NExpression& rhs) : op(op), rhs(rhs) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Unary operator Expression: " << std::endl;
        this->rhs.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NDataConversion : public NExpression {
protected:
    int resultingType;
    int previousType;
public:
    NIdentifier& type;
    NExpression& rhs;
    NDataConversion(NIdentifier& type, NExpression& rhs) : type(type), rhs(rhs) {}
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Data Conversion Expression: " << std::endl;
        this->type.printString(spaces + 1);
        this->rhs.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual int resultType(NBlock& currentBlock);
    virtual bool validate(std::string& error, NBlock& currentBlock);
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
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NBlock : public NExpression {
protected:
    VariableList variables;
    StatementList functions;
    
public:
    StatementList statements;
    const std::string printf = "printf";
    const std::string gets = "gets";
    NBlock() { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Block Expression: " << std::endl;
        
        for(StatementIterator it = this->statements.begin(); it != this->statements.end(); ++it) {
            (*it)->printString(spaces + 1);
        }
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual bool validate(std::string& error, NBlock& currentBlock);
    inline void separateVariablesAndFunctions() {
        
        for (StatementIterator it = statements.begin(); it != statements.end(); it++) {
            std::string name = typeid((**it)).name();
            if (name.find("NVariableDeclaration") != std::string::npos) {
                variables.push_back((NVariableDeclaration *)(*it));
            }
            
            if (name.find("NFunctionDeclaration") != std::string::npos) {
                functions.push_back((*it));
            }
        }
    }
    inline void addVariable(NVariableDeclaration * variable) { variables.push_back(variable); }
    inline VariableList getVariables() { return variables; }
    inline StatementList getFunctions() { return functions; }
    inline NBlock& copy() {
        NBlock *newBlock = new NBlock();
        for (StatementIterator it = statements.begin(); it != statements.end(); it++) {
            if (!(*it)->wasGenerated()) {
                newBlock->statements.push_back(*it);
            }
        }
        newBlock->separateVariablesAndFunctions();
        return *newBlock;
    }
    inline bool contains(std::function<bool(NStatement *)> function) {
        bool cont = false;
        for (StatementIterator it = statements.begin(); it != statements.end(); it++) {
            cont |= function(*it);
        }
        return cont;
    }
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
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NIfStatement : public NStatement {
public:
    NExpression& expression;
    NBlock& block;
    NBlock* elseBlock = NULL;
    NIfStatement(NExpression& expression, NBlock& block) :
    expression(expression), block(block) {}
    NIfStatement(NExpression& expression, NBlock& block, NBlock* elseBlock) :
    expression(expression), block(block), elseBlock(elseBlock) {}
    
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "If Statement: " << std::endl;
           
        this->expression.printString(spaces + 1);
        this->block.printString(spaces + 1);
        
        if (this->elseBlock != NULL) {
            std::cout << std::string(spaces, '\t') << "else Statement: " << std::endl;
            this->elseBlock->printString(spaces + 1);
        }
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NWhileStatement : public NStatement {
public:
    NExpression& expression;
    NBlock& block;
    NWhileStatement(NExpression& expression, NBlock& block) : expression(expression), block(block) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "While Statement: " << std::endl;
        
        this->expression.printString(spaces + 1);
        this->block.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NReturnStatement: public NStatement {
public:
    NExpression& expression;
    NReturnStatement(NExpression& expression) :
        expression(expression) { }
    inline void printString(int spaces) {
        std::cout << std::string(spaces, '\t') << "Return Statement: " << std::endl;
        
        this->expression.printString(spaces + 1);
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
    virtual bool validate(std::string& error, NBlock& currentBlock);
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
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

class NFunctionDeclaration : public NStatement {
protected:
    NReturnStatement* returnStatement = NULL;
    int noOfReturns;
public:
    NIdentifier& type;
    NIdentifier& id;
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(NIdentifier& type, NIdentifier& id,
            VariableList& arguments, NBlock& block) :
        type(type), id(id), arguments(arguments), block(block) {
            
            this->block.separateVariablesAndFunctions();
            for(VariableIterator it = arguments.begin(); it != arguments.end(); it++) {
                this->block.addVariable(*it);
            }
            for (StatementIterator it = block.statements.begin(); it != block.statements.end(); it++) {
                std::string name = typeid((**it)).name();
                if (name.find("NReturnStatement") != std::string::npos) {
                    if (returnStatement == NULL)
                        returnStatement = (NReturnStatement *)(*it);
                    noOfReturns++;
                }
            }
        }
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
    virtual bool validate(std::string& error, NBlock& currentBlock);
};

extern NFunctionDeclaration *echod;
extern NFunctionDeclaration *echob;
extern NFunctionDeclaration *echoi;
#endif /* semantics_h */
