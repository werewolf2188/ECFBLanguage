%{
#include <string>
#include "semantics.hpp"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int analyze_tokens();
%}
  
/*** Rule Section ***/
%%
[ \t\n]             ;// { return 0; }
[\S]                ;
[a-zA-Z][a-zA-Z0-9\_]*   { SAVE_TOKEN; printf("%s its an identifier\n", yytext); return TIDENTIFIER; }
[0-9]+                  { SAVE_TOKEN; printf("%s its an integer \n", yytext); return TINTEGER; }
[0-9]+\.[0-9]+          { SAVE_TOKEN; printf("%s its a double \n", yytext); return TDOUBLE; }
"="                     { printf("its an assignment sign \n"); return TOKEN(TEQUAL); }
"=="                    { printf("its an equals sign \n"); return TOKEN(TCEQ); }
"!="                    { printf("its a not equals sign \n"); return TOKEN(TCNE); }
"<"                     { printf("its a less than sign \n"); return TOKEN(TCLT); }
"<="                    { printf("its a less or equals than sign \n"); return TOKEN(TCLE); }
">"                     { printf("its a less than sign \n"); return TOKEN(TCGT); }
">="                    { printf("its a less or equals than sign \n"); return TOKEN(TCGE); }
"("                     { printf("its an opening parenthesis \n"); return TOKEN(TLPAREN); }
")"                     { printf("its a closing parenthesis \n"); return TOKEN(TRPAREN); }
"{"                     { printf("its an opening key brackets \n"); return TOKEN(TLBRACE); }
"}"                     { printf("its a closing key brackets \n"); return TOKEN(TRBRACE); }
","                     { printf("its a comma \n"); return TOKEN(TCOMMA); }
"."                     { printf("its a dot \n"); return TOKEN(TDOT); }
"+"                     { printf("its a plus sign \n"); return TOKEN(TPLUS); }
"-"                     { printf("its a minus sign \n"); return TOKEN(TMINUS); }
"*"                     { printf("its a multiplication sign \n"); }
"/"                     { printf("its a division sign \n"); return TOKEN(TMUL); }
\".*\"                  { printf("its a string \n"); return TOKEN(TDIV); }
.                       { printf("This is not a correct token\n"); yyterminate(); }

%%
  
/*** Code Section ***/
/// Need to analyze why this has to be extern
int yywrap() { return 1; } 
int analyze_tokens() {
  
// Explanation:
// yywrap() - wraps the above rule section
/* yyin - takes the file pointer
          which contains the input*/
/* yylex() - this is the main flex function
          which runs the Rule Section*/
// yytext is the text in the buffer
  
// Uncomment the lines below
// to take input from file
// FILE *fp;
// char filename[50];
// printf("Enter the filename: \n");
// scanf("%s",filename);
// fp = fopen(filename,"r");
// yyin = fp;
  
    printf("Write any word \n");
    yylex();
    return 0;
}
