%{
#include <string>
extern "C" int analyze_tokens();
%}
  
/*** Rule Section ***/
%%
[ \t\n]             ;// { return 0; }
[\s]                ;
[a-zA-Z][a-zA-Z0-9\_]*   { printf("%s its an identifier\n", yytext); }
[0-9]+              { printf("%s its an integer \n", yytext); }
[0-9]+\.[0-9]+      { printf("%s its a double \n", yytext); }
"="                 { printf("its an equals sign \n"); }
"="                 { printf("its an assignment sign \n"); }
"=="                { printf("its an equals sign \n"); }
"!="                { printf("its a not equals sign \n"); }
"<"                 { printf("its a less than sign \n"); }
"<="                { printf("its a less or equals than sign \n"); }
">"                 { printf("its a less than sign \n"); }
">="                { printf("its a less or equals than sign \n"); }
"("                 { printf("its an opening parenthesis \n"); }
")"                 { printf("its a closing parenthesis \n"); }
"{"                 { printf("its an opening key brackets \n"); }
"}"                 { printf("its a closing key brackets \n"); }
","                 { printf("its a comma \n"); }
"."                 { printf("its a dot \n"); }
"+"                 { printf("its a plus sign \n"); }
"-"                 { printf("its a minus sign \n"); }
"*"                 { printf("its a multiplication sign \n"); }
"/"                 { printf("its a division sign \n"); }
"\""                { printf("its a quotes sign \n"); }
.                   { printf("This is not a correct token\n"); yyterminate(); }

%%
  
/*** Code Section ***/
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
