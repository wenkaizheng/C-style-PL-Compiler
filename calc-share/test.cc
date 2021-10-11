#include <iostream>
#include "globals.h"


int main(int argc, char* argv[]){


    TreeNode * root = statements();
    if(root == NULL){
        printf("no declarations\n");
    }else{
        cout << "10th\n";
        ast_string(root, 0);
        free_memory(root);
    }
    return 0;
}