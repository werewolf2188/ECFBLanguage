%{
    extern int yylex();
    void yyerror(const char *s) { }
%}

%start program

%%

program:
        ;

%%
