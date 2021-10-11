#include <stdlib.h>
#include <iostream>
#include "globals.h"

/* Syntax Analyzer for the calculator language */

static TokenType token; /* holds current token */
string string_val(int);
void spaces(int);
TreeNode* construct_block();
TreeNode* construct_statements();
TreeNode* construct_one_statement();
TreeNode* construct_local_var();
TreeNode* construct_parameters();
TreeNode* construct_while();
TreeNode* construct_if();
TreeNode* construct_break();
TreeNode* construct_return_and_out();
TreeNode* construct_expression();
TreeNode* construct_expression_helper();
TreeNode* construct_comparison();
TreeNode* construct_id(TreeNode*);
TreeNode *exp_prime(TreeNode*);
TreeNode *term_prime(TreeNode*);
TreeNode* arith_term();
TreeNode* arith_expr();

string string_val(int v){
    if (v == VOID){
        return "void";
    }else if(v == INT){
        return "int";
    }else if(v == ASSIGN){
        return "Assign";
    }else if (v == PLUS){
        return  "+";
    }else if (v== SUB){
        return "-";
    }else if ( v== TIMES){
        return "*";
    }else if ( v== DIV){
        return "/";
    }else if (v == QUE){
        return "INPUT";
    }else if (v == GREATHAN){
        return ">=";
    }else if (v == GREAT){
        return ">";
    }else if ( v== LESSTHAN){
        return "<=";
    }else if (v == LESS){
        return "<";
    }else if ( v == NOTEQUAL){
        return "!=";
    }else if ( v == EQUAL){
        return "==";
    }else if ( v == ME){
        return "!";
    }
}
void free_memory(TreeNode* root){
    if (! root){
        return;
    }
    TreeNode* prev = NULL;
    TreeNode* copy = root;
    while(copy){
        for(int i = 0; i<MAXCHILDREN; i++){
            free_memory(copy->child[i]);
        }
        prev = copy;
        copy = copy->next;
        delete prev;
    }
}
void spaces(int num){
   //cout << "58th\n";
    for(int i = 0; i< num; i++){
        cout << ' ';
    }
}
void ast_string(TreeNode* root, int space){
    //cout << root->op << endl;
    if (root->op == NUM){
        spaces(space);
        cout << "Const: " << root->val << endl;
    }
    else if (root->op == VAR){
        spaces(space);
        cout << "Var: " << root->id;
        if (root->array){
            cout << "[]" << endl;
            spaces(space+1);
            cout << "index expression: " << endl;
            ast_string(root->child[0],space+2);
        }else{
            cout << "\n";
        }
    }
    else if (root->op == BLOCK){
        spaces(space);
        cout << "BLOCK" << endl;
        TreeNode* c = root->child[0];
        if ( c == NULL){
            spaces(1+space);
            cout << "Local var declaration: " << endl;
        }
        while(c){
            spaces(1+space);
            cout << "Local var declaration: " << c->id << " " << "(type: "  << string_val(c->type)  << ")";
            if (c->array){
                cout << "[" << c->size << "]";
            }
            cout << "\n";
            c = c->next;
        }
        if (root->child[1]){
            spaces(1+space);
            cout << "Local statements: "<< endl;
            ast_string(root->child[1],space +2);
        }else{
            cout << "Local statements: "<< endl;
        }
    }
    else if (root->op == VDECLARE){
        cout << "Global var declaration: " << root->id << " " << "(type: "  << string_val(root->type)  << ")";
        if (root->array){
            cout << "[" << root->size << "]";
        }
        cout << "\n";
    }
    else if (root->op == FDECLARE){
        cout << "Global function declaration: " << root->id << " " << "(type: "  << string_val(root->type)  << ")\n";
        // print parameter list
        TreeNode* c = root->child[0];
        if ( c == NULL){
            cout << "Function parameter: " << endl;
        }
        while(c){
            cout << "Function parameters: " << c->id << " " << "(type: "  << string_val(c->type)  << ")";
            if (c->array){
                cout << "[]";
            }
            cout << "\n";
            c = c ->next;
        }
        ast_string(root->child[1],1);
    }
    else if (root->op == WHILE){
        spaces(space);
        cout << "Repetition code" << endl;
        spaces(space+1);
        cout << "while condition" << endl;
        ast_string(root->child[0],space+2);
        if (root->child[1]){
            spaces(space+1);
            cout << "while code" << endl;
            ast_string(root->child[1],space+2);
        }
    }
    else if (root->op == IF){
        spaces(space);
        cout << " code" << endl;
        spaces(space+1);
        cout << "if  condition" << endl;
        ast_string(root->child[0],space+2);
        if (root->child[1]){
            spaces(space+1);
            cout << "if code" << endl;
            ast_string(root->child[1],space+2);
        }
        if (root->child[2]){
            spaces(space+1);
            cout << "else code" << endl;
            ast_string(root->child[2],space+2);
        }
    }
    else if (root->op == OUT){
        spaces(space);
        cout << "Out" << endl;
        if (root->child[0]){
            ast_string(root->child[0],space+1);
        }
    }
    else if (root->op == RETURN){
        spaces(space);
        cout << "Return" << endl;
        if (root->child[0]){
            ast_string(root->child[0],space+1);
        }
    }
    else if (root->op == BREAK){
        spaces(space);
        cout << "Break" << endl;
    }
    else if (root->op == CFUNCTION){
        spaces(space);
        cout << "Call: " << root->id << endl;
        spaces(space+1);
        cout << "Call functions arguments: " << endl;
        if (root->child[0]){
            ast_string(root->child[0],2 + space);
        }
    }else{
        // all  + - * / = >= <= < > ! !=
        spaces(space);
        cout << "Op: "<< string_val(root->op) << endl;
        for(int i = 0;i<MAXCHILDREN;i++){
            if (root->child[i]){
                ast_string(root->child[i],space+1);
            }
        }
    }
    if (root->next){
        // global function and var declare
        ast_string(root->next,space);
    }
}
/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode *newNode(TokenType tType) {
    TreeNode *t = new TreeNode();
    // The above line is analogous to the C statement
    // TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));

    int i;
    if (t == NULL) {
        cerr << "Out of memory error at line %d\n";
        exit(1);
    } else {
        for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->op = tType.TokenClass;
        t->next = NULL;
        t->array = false;
        t->type = -1;
        t->pos = -1;
        t->size = -1;

    }
    return t;
}

static void advance(int expected) {
    cout << "225th " << token.TokenString << endl;
    if (token.TokenClass == expected) token = getToken();
    else {
        cerr << "unexpected token -> " << token.TokenString << endl;
    }
}
TreeNode* construct_argument(){
    TreeNode* h = NULL, *t = NULL;
    while(token.TokenClass!=RPAREN){
        TreeNode* p = construct_expression_helper();
        if (p == NULL){
            continue;
        }
        if (h == NULL){
            h = t  = p;
        }else{
            t->next = p;
            t = t->next;
        }
        if (token.TokenClass == COM){
            advance(token.TokenClass);
        }else if (token.TokenClass != RPAREN){
            cerr << "error[missing a ) for function call]" << endl;;
            exit(1);

        }
    }
    return h;
}

TreeNode* construct_id(TreeNode* left){
    // function call or array or var
    left->op = VAR;
    if (token.TokenClass == LPAREN){
        left->op = CFUNCTION;
        advance(token.TokenClass);
        left->child[0] = construct_argument();
        advance(RPAREN);
    }else if (token.TokenClass == LEFT){
        advance(token.TokenClass);
        left->array = true;
        // the size can  be an expression in here
        left->child[0] = construct_expression_helper();
        if(token.TokenClass == RIGHT){
            advance(token.TokenClass);
        }else{
            cerr << "error[missing a ] for array declaration in expression]" << endl;
            exit(1);
        }
    }
    return left;
}

TreeNode* construct_expression_helper() {
    TreeNode* rv = NULL;
    if (token.TokenClass == ID){
        TreeNode* t = newNode(token);
        t->id = token.TokenString;
        advance(token.TokenClass);
        TreeNode* tmp = construct_id(t);
        //return var_op(tmp);
        TreeNode* rv = NULL;
        if (token.TokenClass == PLUS || token.TokenClass == SUB){
            return  exp_prime(tmp);
        }else if (token.TokenClass == TIMES || token.TokenClass == DIV){
            return  exp_prime(term_prime(tmp));
        }else if(token.TokenClass == ASSIGN){
            rv = newNode(token);
            advance(token.TokenClass);
            rv->child[0] = tmp;
            rv->child[1] = construct_expression_helper();
            return rv;
        }
        return tmp;
    }else{
        rv = arith_expr();
    }
    return rv;

}
TreeNode* construct_expression() {
    cout << "278th\n";
    if (token.TokenClass != SEMI) {
        TreeNode *rv = construct_expression_helper();
        if (token.TokenClass != SEMI) {
            cout << token.TokenClass << endl;
            cerr << "error[missing a ; at the expression end]" << endl;
            exit(1);
        }
        advance(token.TokenClass);
        cout << "322th\n";
        return rv;
    } else {
        advance(token.TokenClass);
        return NULL;
    }
}

TreeNode *factor() {
    TreeNode* rv = newNode(token);
    cout << token.TokenClass << endl;
    cout << token.TokenString << endl;
    // expression with ()
    if (token.TokenClass == LPAREN){
        delete rv;
        advance(token.TokenClass);
        rv = construct_expression_helper();
        advance(RPAREN);
    }else if (token.TokenClass == NUM){
        rv->val = stoi(token.TokenString);
        advance(token.TokenClass);
    }else if (token.TokenClass == QUE){
        rv->id = token.TokenString;
        advance(token.TokenClass);
    }else if (token.TokenClass == ID){
        rv->id = token.TokenString;
        advance(token.TokenClass);
        rv = construct_id(rv);
    }else{
        cout << token.TokenClass << "\n";
        cerr << "error[factor element is not valid]" << endl;
        exit(1);
    }
    return rv;
}

TreeNode *exp_prime(TreeNode *left) {
    if ((token.TokenClass == PLUS)
        || (token.TokenClass == SUB)
            ) {
        TreeNode *p = newNode(token);
        advance(token.TokenClass);
        cout << "357th\n";
        if (p != NULL) {
            p->child[0] = left;
            p->child[1] = arith_term();
            return exp_prime(p);
        }
    } else {
        cout << "364th\n";
        return left;
    }
}

TreeNode *term_prime(TreeNode *left) {
    if ((token.TokenClass == TIMES)
        || (token.TokenClass == DIV)
            ) {
        TreeNode *p = newNode(token);
        advance(token.TokenClass);
        cout << "374th\n";
        if (p != NULL) {
            p->child[0] = left;
            p->child[1] = factor();
            return term_prime(p);
        }
    } else {
        cout << "381th\n";
        return left;
    }
}

//arith-term → arith-factor | arith-term mulop arith-factor
TreeNode* arith_term(){
   // cout << "377th\n";
    TreeNode* t = factor();
   // cout << "378th\n";
    return term_prime(t);
}
// arith-expr → arith-term | arith-expr addop arith-term
TreeNode* arith_expr(){
    TreeNode* t = arith_term();
    return exp_prime(t);
}
// comparison → arith-expr relop arith-expr | ! ( comparison )
TreeNode* construct_comparison(){
    // not case
    TreeNode* rv = NULL;
    if (token.TokenClass == ME){
        rv = newNode(token);
        advance(token.TokenClass);
        advance(LPAREN);
        rv->child[0] = construct_comparison();
        advance(RPAREN);
    }else{
        // arith_expr compare arith_expr
        TreeNode* t = arith_expr();
        if (token.TokenClass == LESS || token.TokenClass == LESSTHAN
    || token.TokenClass == GREAT || token.TokenClass == GREATHAN ||
    token.TokenClass == EQUAL || token.TokenClass == NOTEQUAL){
            cout << "418th\n";
            rv = newNode(token);
            advance(token.TokenClass);
            rv->child[0] = t;
            rv->child[1] = arith_expr();
        }else{
            cerr << "error[relop value is incorrect in comparison]" << endl;
            exit(1);
        }
    }
    return rv;
}
TreeNode* construct_if(){
    TreeNode* rv = newNode(token);
    advance(token.TokenClass);
    // to see if there is a (
    advance(LPAREN);
    cout << "434th\n";
    rv->child[0] = construct_comparison();
    cout << "435th\n";
    advance(RPAREN);
    rv->child[1] = construct_one_statement();
    // more statement
    cout << "438th\n";
    if (token.TokenClass == ELSE){
        advance(token.TokenClass);
        rv->child[2] = construct_one_statement();
    }
    return rv;
}
TreeNode* construct_while(){
    TreeNode* rv = newNode(token);
    advance(token.TokenClass);
    advance(LPAREN);
    rv->child[0] = construct_comparison();
    advance(RPAREN);
    rv->child[1] = construct_one_statement();
    return rv;
}
TreeNode* construct_return_and_out(){
    TreeNode* rv = newNode(token);
    advance(token.TokenClass);
    rv->child[0] = construct_expression();
    return rv;
}
TreeNode* construct_break(){
    TreeNode* rv = newNode(token);
    advance(token.TokenClass);
    return rv;
}
TreeNode* construct_block(){
    TreeNode* rv = newNode(token);
    rv->op = BLOCK;
    if (token.TokenClass == INT || token.TokenClass == VOID) {
        rv->child[0] = construct_local_var();
    }
    rv->child[1] = construct_statements();
    return rv;
}
TreeNode* construct_one_statement(){
    TreeNode* rv = NULL;
    if (token.TokenClass == STA){
        advance(token.TokenClass);
        rv = construct_block();
        advance(END);
    }else if (token.TokenClass == WHILE){
        rv = construct_while();
    }else if (token.TokenClass == IF){
        rv = construct_if();
    }else if (token.TokenClass == BREAK){
        rv = construct_break();
    }else if (token.TokenClass == RETURN){
        rv =  construct_return_and_out();
    }else if (token.TokenClass == OUT){
        rv = construct_return_and_out();
    }else{
        rv = construct_expression();
    }
    return rv;
}
TreeNode* construct_statements(){
    TreeNode* h = NULL, *t = NULL;
    TreeNode* p = NULL;
    // until we find a }
    while(token.TokenClass != END && token.TokenClass!= ENDFILE){
        TreeNode* rv = construct_one_statement();
        if (rv == NULL){
            continue;
        }else{
            if (h == NULL){
                h = t = rv;
            }else{
                t->next = rv;
                t = t->next;
            }
        }
    }
    if(token.TokenClass == ENDFILE){
        cerr << "lack } in function block.\n" << endl;
        exit(1);
    }
    return h;
}
TreeNode* construct_local_var(){
    TreeNode* h = NULL, *t = NULL;
    while(token.TokenClass == INT || token.TokenClass == VOID){
        TreeNode* p = newNode(token);
        if (h == NULL){
            h = t = p;
        }else{
            t->next = p;
            t = t-> next;
        }
        p->op = LVDECLARE;
        p->type = token.TokenClass;
        advance(token.TokenClass);
        if (token.TokenClass == ID){
           p->id = token.TokenString;
           advance(token.TokenClass);
           if (token.TokenClass == LEFT){
               advance(token.TokenClass);
               p->array = true;
               if (token.TokenClass == NUM){
                   p->size = stoi(token.TokenString);
                   advance(token.TokenClass);
                   if (token.TokenClass == RIGHT){
                       advance(token.TokenClass);
                       if (token.TokenClass == SEMI){
                           // loop next op
                           advance(token.TokenClass);
                       }else{
                           cerr  <<  "error[missing a ; to end array declaration]" <<endl;
                           exit(1);
                       }
                   }else{
                       cerr << "error[missing a ] to declare size for array]" << endl;
                       exit(1);
                   }
               }else{
                   cerr << "error [ the size of global array should be a number ]" << endl;
                   exit(1);
               }
           }
           else if (token.TokenClass == SEMI){
               advance(token.TokenClass);
           }else{
               cerr << "error[global var or function declaration is incorrect]" << endl;
               exit(1);
           }
        }else{
            cerr << "error[missing a id for local var declaration]" <<endl;
            exit(1);
        }
    }
    return h;
}
TreeNode* construct_parameters(){
    TreeNode* h = NULL, *t = NULL;
    if (token.TokenClass == RPAREN){
        cerr << "error [missing a function parameter]" << endl;
        exit(1);
    }
    while(token.TokenClass != RPAREN){
        TreeNode* p = newNode(token);
        if (h == NULL){
            h = t = p;
        }else{
            t->next = p;
            t = t->next;
        }
        if (token.TokenClass == VOID || token.TokenClass == INT){
            p->op = FPARAMETER;
            p->type = token.TokenClass;
            advance(token.TokenClass);
            if(token.TokenClass == ID){
                p->id = token.TokenString;
                advance(token.TokenClass);
                // ,
                if (token.TokenClass == COM){
                    advance(token.TokenClass);
                }
                else if (token.TokenClass == LEFT){
                    p->array = true;
                    advance(token.TokenClass);
                    if (token.TokenClass == RIGHT){
                        advance(token.TokenClass);
                        if (token.TokenClass == COM){
                            advance(token.TokenClass);
                        }
                        else if(token.TokenClass != RPAREN){
                            cerr << "error[missing a comma or ) in parameter list]" << endl;
                            exit(1);
                        }
                    }else{
                        cerr << "error[missing a ] to end a array in parameter list]" << endl;
                        exit(1);
                    }
                }
                else if (token.TokenClass != RPAREN){
                    cerr << "error missing a ) to end parameter list" << endl;
                    exit(1);
                }
            }else if ( token.TokenClass == RPAREN && p->type != VOID){
                cerr << "error [missing a id for function parameter]" << endl;
                exit(1);
            }
        }else{
            cerr << "error [function parameter type only support int and void]" <<endl;
            exit(1);
        }
    }
    return h;
}
/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function statements returns the newly 
 * constructed syntax tree
 */
// global var/array and function declaration
TreeNode *statements(void) {
    TreeNode *h = NULL, *t = NULL;

    /* prime the token */
    token = getToken();

    while (token.TokenClass != ENDFILE) {
        TreeNode *p = newNode(token);

        /* create a linked list of statements */
        if (h == NULL)  /* initially empty */
            h = t = p;
        else { /* generally non-empty */
            t->next = p;
            t = t->next;
        }
        // C-- program should start with function declare or var declare
        if (token.TokenClass == INT || token.TokenClass == VOID) { /* assignment */
            p->type = token.TokenClass;
            advance(token.TokenClass);
            // we need an id
            if (token.TokenClass == ID){
               p->id = token.TokenString;
               advance(token.TokenClass);
               // array declaration []
               if (token.TokenClass == LEFT){
                   p->array = true;
                   p->op = VDECLARE;
                   advance(token.TokenClass);
                   // size for array
                   if (token.TokenClass == NUM){
                       p->size = stoi(token.TokenString);
                       advance(token.TokenClass);
                       if (token.TokenClass == RIGHT){
                           advance(token.TokenClass);
                           if (token.TokenClass == SEMI){
                               // loop next op
                               advance(token.TokenClass);
                           }else{
                               cerr  <<  "error[missing a ; to end array declaration]" <<endl;
                               exit(1);
                           }
                       }else{
                           cerr << "error[missing a ] to declare size for array]" << endl;
                           exit(1);
                       }
                   }else{
                       cerr << "error [ the size of global array should be a number ]" << endl;
                       exit(1);
                   }
               }
               // (
               else if(token.TokenClass == LPAREN ){
                    p->op = FDECLARE;
                    advance(token.TokenClass);
                    p->child[0] = construct_parameters();
                    // ){
                    advance(RPAREN);
                    if (token.TokenClass == STA){
                        advance(token.TokenClass);
                        p->child[1] = construct_block();
                        // }
                        advance(END);
                    }else{
                        cerr << "error[missing a } for block" << endl;
                        exit(1);
                    }
               }else if (token.TokenClass == SEMI){
                   p->op = VDECLARE;
                   advance(token.TokenClass);
               }
               else{
                   cerr << "error[global var or function declaration is incorrect]" << endl;
                   exit(1);
               }
            }else{
                cerr << "error[missing a id for global var or function declaration]" <<endl;
                exit(1);
            }
        }

        else {
            cerr << "error [global var or function declaration type only support int and void]" <<endl;
            exit(1);
        }


    }
    return h;
}
