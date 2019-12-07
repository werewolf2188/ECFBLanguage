%{
#include <string>
#include "semantics.hpp"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int analyze_tokens();
extern "C" void removeChar(char *str, char garbage);
%}
  
/*** Rule Section ***/
%%
[ \t\n]             ;
[\S]                ;
\/\/.*              ; //Comments in a one line
"return"                { return TOKEN(TRETURN); }
"true"|"false"          { SAVE_TOKEN; return TBOOLEAN; }
[a-zA-Z][a-zA-Z0-9\_]*  { SAVE_TOKEN; return TIDENTIFIER; }
[0-9]+                  { SAVE_TOKEN; return TINTEGER; }
[0-9]+\.[0-9]+          { SAVE_TOKEN; return TDOUBLE; }
"="                     { return TOKEN(TEQUAL); }
"=="                    { return TOKEN(TCEQ); }
"!="                    { return TOKEN(TCNE); }
"<"                     { return TOKEN(TCLT); }
"<="                    { return TOKEN(TCLE); }
">"                     { return TOKEN(TCGT); }
">="                    { return TOKEN(TCGE); }
"!"                     { return TOKEN(TNOT); }
"("                     { return TOKEN(TLPAREN); }
")"                     { return TOKEN(TRPAREN); }
"{"                     { return TOKEN(TLBRACE); }
"}"                     { return TOKEN(TRBRACE); }
","                     { return TOKEN(TCOMMA); }
"."                     { return TOKEN(TDOT); }
"+"                     { return TOKEN(TPLUS); }
"-"                     { return TOKEN(TMINUS); }
"*"                     { return TOKEN(TMUL);  }
"/"                     { return TOKEN(TDIV); }
"%"                     { return TOKEN(TREMAIN); }
\".*\"                  { removeChar(yytext, '"'); printf(yytext); SAVE_TOKEN; return TSTRING; }
.                       { printf("This is not a correct token\n"); yyterminate(); }

%%
  
/*** Code Section ***/
int yywrap() { return 1; }

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

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
