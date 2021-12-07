#include <iostream>
#include "globals.h"


int main(int argc, char* argv[]){


    TreeNode * root = construct_declarations();
    // traverse the tree and set the weight
    ast_string(root,0);
    symbol_table* st = analyze(root);
    if (st == NULL){
        cerr << "symbol table is empty\n";
        return 1;
    }
    code_generation(root,st);
    print_code();
    free_memory(root);
    free_collector();
    free_memory(st);
    return 0;
}