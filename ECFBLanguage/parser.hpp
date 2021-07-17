/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TIDENTIFIER = 258,
     TINTEGER = 259,
     TDOUBLE = 260,
     TBOOLEAN = 261,
     TSTRING = 262,
     TCEQ = 263,
     TCNE = 264,
     TCLT = 265,
     TCLE = 266,
     TCGT = 267,
     TCGE = 268,
     TEQUAL = 269,
     TNOT = 270,
     TAND = 271,
     TOR = 272,
     TLPAREN = 273,
     TRPAREN = 274,
     TLBRACE = 275,
     TRBRACE = 276,
     TCOMMA = 277,
     TDOT = 278,
     TPLUS = 279,
     TMINUS = 280,
     TMUL = 281,
     TDIV = 282,
     TREMAIN = 283,
     TRETURN = 284,
     TIF = 285,
     TELSE = 286,
     TWHILE = 287
   };
#endif
/* Tokens.  */
#define TIDENTIFIER 258
#define TINTEGER 259
#define TDOUBLE 260
#define TBOOLEAN 261
#define TSTRING 262
#define TCEQ 263
#define TCNE 264
#define TCLT 265
#define TCLE 266
#define TCGT 267
#define TCGE 268
#define TEQUAL 269
#define TNOT 270
#define TAND 271
#define TOR 272
#define TLPAREN 273
#define TRPAREN 274
#define TLBRACE 275
#define TRBRACE 276
#define TCOMMA 277
#define TDOT 278
#define TPLUS 279
#define TMINUS 280
#define TMUL 281
#define TDIV 282
#define TREMAIN 283
#define TRETURN 284
#define TIF 285
#define TELSE 286
#define TWHILE 287




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE

{
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
/* Line 1529 of yacc.c.  */

	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

