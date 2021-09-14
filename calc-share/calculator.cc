#include <iostream>
#include "globals.h"

int main(void) {
  cout << EOF << endl;
  TreeNode* icode = statements(); // top-level function for parser
  if (icode == NULL) {
    cerr << "no top-level expression\n";
    exit(1);
  }
  analyze(icode);
  codeGenStmt(icode);
  printCode();
  return 0;
}

