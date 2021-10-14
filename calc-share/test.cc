#include <iostream>
#include "globals.h"


int main(int argc, char* argv[]){


    TreeNode * root = construct_declarations();
    if(root == NULL){
        printf("no declarations\n");
    }else{
        ast_string(root, 0);
        free_memory(root);
    }
    return 0;
}