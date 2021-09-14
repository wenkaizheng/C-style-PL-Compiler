#include <stdlib.h>
#include <iostream>
#include "globals.h"

/* Syntax Analyzer for the calculator language */

static TokenType token; /* holds current token */

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newNode(TokenType tType) {
  TreeNode * t = new TreeNode();
  // The above line is analogous to the C statement
  // TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));

  int i;
  if (t==NULL) {
    cerr << "Out of memory error at line %d\n";
    exit(1);
  }
  else {
    for (i=0;i < MAXCHILDREN;i++) t->child[i] = NULL;
    t->op = tType.TokenClass;
  }
  return t;
}

static void advance(int expected) {
  if (token.TokenClass == expected) token = getToken();
  else {
    cerr << "unexpected token -> " << token.TokenString << endl; 
  }
}

TreeNode * exp(void); // forward declaration

TreeNode * factor(void) {
  TreeNode * t = NULL;
  switch (token.TokenClass) {
  case NUM :
    t = newNode(token);
    if ((t!=NULL) && (token.TokenClass==NUM))
      t->val = atoi(token.TokenString.c_str());
    advance(NUM);
    break;
  case ID :
    t = newNode(token);
    if ((t!=NULL) && (token.TokenClass==ID))
      t->id = token.TokenString[0];
    advance(ID);
    break;
  case LPAREN :
    advance(LPAREN);
    t = exp();
    advance(RPAREN);
    break;
  default:
    cerr << "unexpected token -> " << token.TokenString << endl;
    exit(1);
    break;
  }
  return t;
}

TreeNode * exp_prime(TreeNode * left) {
  if ((token.TokenClass==PLUS) 
      || (token.TokenClass==SUB)
      ) {
    TreeNode * p = newNode(token);
    
    advance(token.TokenClass);
    if (p!=NULL) {
      p->child[0] = left;
      p->child[1] = factor();
      return exp_prime(p);
    }
  }
  else return left;
}

TreeNode * exp(void) { 
  TreeNode * t = factor();
  return exp_prime(t);
}


/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function statements returns the newly 
 * constructed syntax tree
 */
TreeNode * statements(void) { 
  TreeNode *h = NULL, *t = NULL;

  /* prime the token */
  token = getToken();
  
  while (token.TokenClass != ENDFILE) {
    TreeNode * p = newNode(token);
    
    /* create a linked list of statements */
    if (h == NULL)  /* initially empty */
      h = t = p;
    else { /* generally non-empty */
      t->child[1] = p;
      t = p;
    }
    
    if (token.TokenClass==OUT) {
      advance(token.TokenClass);
      p->child[0] = exp();
    }
    else if (token.TokenClass==ID) { /* assignment */
      p->op = ASSIGN;
      p->id = token.TokenString[0];
      
      advance(token.TokenClass);
      if (token.TokenClass != ASSIGN) {
	cerr << "Invalid assignment statement " 
	     << p->id << " " << token.TokenString << endl;
	exit(1);
      }
      advance(ASSIGN);
      p->child[0] = exp();
    }
    else {
      cerr << "non-lvalue assignment\n";
      exit(1);
    }
    
    if (token.TokenClass == SEMI) {
      /* more statements */
      token = getToken();
    }
    else if (token.TokenClass != ENDFILE) {
      cerr << "unexpected end of statement: " << token.TokenString << "\n";
      exit(1);
    }
  }
  return h;
}
