#include <iostream>
#include "globals.h"

/* Code Generation for IBM */

static CodeType codeArray[CODESIZE];
static int ICounter = 0;
static int C_OFFSET = 997; // the *fixed* address assigned to variable 'a'
static int tempAddr = 100;

void emit(string code, OpCodeType ctype, 
	  int operand1, int operand2, int operand3) {
  codeArray[ICounter].opcode = code;
  codeArray[ICounter].ctype = ctype;
  codeArray[ICounter].rand1 = operand1;
  codeArray[ICounter].rand2 = operand2;
  codeArray[ICounter++].rand3 = operand3;
}

void codeGenExp(TreeNode* icode) {
  /* register 0 for ac */
  /* register 1 for ac1 */

  if (icode != NULL) {
    switch (icode -> op) {
    case NUM: 
      emit("LDC",RM,0,icode->val,0);
      break;
    case ID:
      emit("LD",RM,0,icode -> id - 'a' + C_OFFSET,2);
      break;
    case PLUS:
      codeGenExp(icode -> child[0]);
      emit("ST",RM,0,tempAddr++,2);
      codeGenExp(icode -> child[1]);
      emit("LD",RM,1,--tempAddr,2);
      emit("ADD",RO,0,1,0);
      break;
    case SUB:
      codeGenExp(icode -> child[0]);
      emit("ST",RM,0,tempAddr++,2);
      codeGenExp(icode -> child[1]);
      emit("LD",RM,1,--tempAddr,2);
      emit("SUB",RO,0,1,0);
      break;
    default:
      cerr << "code generation error" << endl;
    }
  }
}

void codeGenStmt(TreeNode* icode) {
  /* register 0 for ac */
  /* register 1 for ac1 */

  while (icode != NULL) {
    switch (icode -> op) {
    case ASSIGN:
      codeGenExp(icode -> child[0]);
      emit("ST",RM,0,icode -> id - 'a' + C_OFFSET,2);
      break;
    case OUT:
      codeGenExp(icode -> child[0]);
      emit("OUT",RO,0,0,0);
      break;
    default:
      cerr << "code generation error" << endl;
    }
    icode = icode -> child[1];
  }
}


void printCode(void) {
  for (int i = 0; i < ICounter; i++) {
    cout << i << ": " 
	 << codeArray[i].opcode << " "
	 << codeArray[i].rand1 << ","
	 << codeArray[i].rand2 
	 << (codeArray[i].ctype == RO? "," : "(")
	 << codeArray[i].rand3;
    if (codeArray[i].ctype == RM) cout << ")";
    cout << endl;
  }
}


/* This version frees every register for every subexpression. 

void codeGenExp(TreeNode* icode) {
  // register 0 for ac 
  // register 1 for ac1 

  if (icode != NULL) {
    switch (icode -> op) {
    case NUM: 
      emit("LDC",RM,0,icode->val,0);
      emit("ST",RM,0,tempAddr++,2);
      break;
    case ID:
      emit("LD",RM,0,icode -> id - 'a' + C_OFFSET,2);
      emit("ST",RM,0,tempAddr++,2);
      break;
    case PLUS:
      for (int i = 0; i < MAXCHILDREN; i++)
	codeGenExp(icode -> child[i]);

      emit("LD",RM,1,--tempAddr,2);
      emit("LD",RM,0,--tempAddr,2);
      emit("ADD",RO,0,0,1);
      emit("ST",RM,0,tempAddr++,2);
      break;
    case SUB:
      for (int i = 0; i < MAXCHILDREN; i++)
	codeGenExp(icode -> child[i]);

      emit("LD",RM,1,--tempAddr,2);
      emit("LD",RM,0,--tempAddr,2);
      emit("SUB",RO,0,0,1);
      emit("ST",RM,0,tempAddr++,2);
      break;
    default:
      cerr << "code generation error" << endl;
    }
  }
}

*/
