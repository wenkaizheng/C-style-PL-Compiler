#include <stdlib.h>

#ifndef _GLOBALS_H
#define _GLOBALS_H

// named token class
#define ENDFILE 256
#define ERROR   257
#define NUM     258
#define PLUS    259
#define TIMES   260
#define LPAREN  261
#define RPAREN  262
#define SUB     263
#define DIV     264
#define ID      265
#define ASSIGN  266
#define OUT     267
#define SEMI    268
#define NOT     269
#define QUE     270
#define LESS    271
#define GREAT   272
#define COM     273
#define STA     274
#define END     275
#define LEFT    276
#define RIGHT   277
#define INT     288
#define VOID    289
#define IF      290
#define ELSE    291
#define  WHILE  292
#define RETURN  293
#define GREATHAN 294
#define LESSTHAN 295
#define NOTEQUAL 296
#define EQUAL   297
#define ME        298
using namespace std;

/* This is C-version of the TokenType class 

typedef struct {
  int TokenClass;  // one of the above
  char* TokenString;
} TokenType; 

*/

class TokenType {
public:
  int    TokenClass;  // one of the above
  string TokenString;
};

#define MAXCHILDREN 2

class TreeNode {
public:
    TreeNode * child[MAXCHILDREN];
    int  op;
    int  val;
    char id;
};

#define ST_SIZE 26 // ST: Symbol Table

#define CODESIZE 100

typedef enum {RO,RM} OpCodeType;

class CodeType {
public:
  string     opcode;
  OpCodeType ctype;
  int rand1;
  int rand2;
  int rand3;
};

/* 
 * Following are the top-level functions for 
 * each of the phases of the compiler.
 */

extern std::string key_word[10];
extern int key_word_class[10];
extern int key_word_class2[4];
TokenType getToken(void);
TreeNode * statements(void);
void analyze(TreeNode*);
void codeGenStmt(TreeNode*);
void emit(string,OpCodeType,int,int,int);
void printCode(void);

#endif
