%{
    #include "semantics.hpp"
    
    NBlock* programBlock;
    extern int yylex();
    void yyerror(const char *s) { }
%}

%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NVariableDeclaration *var_decl;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
    std::string *string;
    int token;
}

%token <string> TIDENTIFIER TINTEGER TDOUBLE TBOOLEAN
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL TNOT
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV TREMAIN
%token <token> TRETURN

%type <ident> ident
%type <expr> numeric boolean expr
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> stmts block
%type <stmt> program stmt var_decl func_decl
%type <token> comparison
%type <token> negative

%left TPLUS TMINUS
%left TMUL TDIV TREMAIN

%start program

%%

program : stmts { programBlock = $1; }
;
        
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
| stmts stmt { $1->statements.push_back($<stmt>2); }
;
        
stmt : var_decl | func_decl
| expr { $$ = new NExpressionStatement(*$1); }
| TRETURN expr { $$ = new NReturnStatement(*$2); }
;

block : TLBRACE stmts TRBRACE { $$ = $2; }
| TLBRACE TRBRACE { $$ = new NBlock(); }
;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
| TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
;

boolean : TBOOLEAN { $$ = new NBoolean($1); delete $1; }
;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
| ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
;

func_decl : ident ident TLPAREN func_decl_args TRPAREN block
  { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
;

func_decl_args : /*blank*/  { $$ = new VariableList(); }
| var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
| func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
;

expr : ident TEQUAL expr { $$ = new NAssignment(*$<ident>1, *$3); }
| ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
| ident { $<ident>$ = $1; }
| numeric
| boolean
| TLPAREN ident TLPAREN expr { $$ = new NDataConversion(*$2, *$4); }
| negative expr { $$ = new NUnaryOperator($1, *$2); }
| expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
| TLPAREN expr TRPAREN { $$ = $2; }
;

call_args : /*blank*/  { $$ = new ExpressionList(); }
| expr { $$ = new ExpressionList(); $$->push_back($1); }
| call_args TCOMMA expr  { $1->push_back($3); }
;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
| TPLUS | TMINUS | TMUL | TDIV | TREMAIN
;

negative : TNOT | TMINUS
;
%%
