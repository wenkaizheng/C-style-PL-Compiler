#include <iostream>
#include "globals.h"


int main(int argc, char* argv[]){


    TreeNode * root = construct_declarations();
    ast_string(root,0);
    symbol_table* st = analyze(root);
    if (st == NULL){
        cerr << "symbol table is empty\n";
        return 1;
    }
    st_string(st,0);
    free_memory(root);
    free_collector();
    free_memory(st);
    return 0;
}