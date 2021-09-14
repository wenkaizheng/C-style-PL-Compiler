#include <iostream>
#include "globals.h"

/* Semantic Analysis: checks for undefind variables */

static bool ST[ST_SIZE] = {
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false 
};

void analyzeExp(TreeNode* icode) {
    /* look for all occurrences of undefined variables */
    if (icode != NULL) {
	if (icode -> op == ID && !ST[icode -> id - 'a']) {
	    cerr << "undefined variable: " << icode->id << endl;
	    exit(1);
	}
	for (int i = 0; i < MAXCHILDREN; i++)
	    analyzeExp(icode -> child[i]);
  }
}

void analyze(TreeNode* icode) {
    /* analyze all the statements in the program */
    while (icode != NULL) {
	if (icode -> op == ASSIGN) { 
	    ST[icode -> id - 'a'] = true;
	}
	analyzeExp(icode -> child[0]);
	icode = icode -> child[1];
    }
}


