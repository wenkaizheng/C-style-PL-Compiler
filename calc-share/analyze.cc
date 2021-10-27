#include <iostream>
#include "globals.h"
#include <set>
#include <vector>
// we need to make sure the block is in current function
// we use start_with function to check the block
string function_name = "";
int function_type = -1;
vector<symbol*> collector;
/* Semantic Analysis: checks for undefind variables */
string string_vals(int v){
    if (v== INT){
        return "int";
    }else if (v == VOID){
        return "void";
    }
}
void free_symbol(symbol_table* st){
    for(auto it = st->coll.begin();it != st->coll.end();it++) {
         parameter* p = it->second->parameters;
         parameter* prev = NULL;
         while(p){
            prev = p;
            p = p->next;
            delete prev;
         }
         delete it->second;
    }
}
void free_memory(symbol_table* st){
   if (!st){
       return;
   }
  // symbol_table* prev = NULL;
   symbol_table* copy = st;
   if(copy){
       free_symbol(copy);
       symbol_table* c = copy->children;
       symbol_table* prev_children = NULL;
       while(c){
           prev_children = c;
           c = c->next;
           //cout << "41th"  << prev_children->scope << "\n";
           free_memory(prev_children);
       }
       cout << "46th " << copy->scope << endl;
       delete copy;

   }
}
void free_collector(){
    for(auto it = collector.begin();it != collector.end();it++) {
        delete *it;
    }
}
symbol_table* analyze_block(TreeNode*, symbol_table*, string,int);
void st_string(symbol_table* st, int space){

    if(st->scope == "Global"){
        if (st->children){
            cout << "22th\n";
            st_string(st->children,space);
        }
        if (st->next){
            cout << "26th\n";
            st_string(st->next,space);
        }
    }
    spaces(space);
    cout << "scope-name is " << st->scope << endl;
    for(auto it = st->coll.begin();it != st->coll.end();it++){
        symbol* s = it->second;
        if(s->declare_type == FDECLARE){
            spaces(space);
            cout << "function type " << string_vals(s->type) << endl;
            spaces(space);
            cout << "body of " << it->first << endl;
            spaces(space);
            cout << "(";
            parameter* p = s->parameters;
            while(p){
                spaces(space);
                cout << string_vals(p->type) << " ";
                if (p->id != ""){
                    cout << p->id;
                }
                if (p->array){
                    cout << "[]";
                }
                if (p->next){
                    cout << ",";
                }
                p = p->next;
            }
            cout << ")" << endl;
        }else if (s->declare_type == VDECLARE || s->declare_type == LVDECLARE){
                spaces(space);
                cout << string_vals(s->type) << " " << it->first;
                if(s->array){
                    spaces(space);
                    cout << "[" << s->size << "]";
                }
                cout <<"\n";
        }
    }
    if(st->scope != "Global"){
        if (st->children){
            st_string(st->children,space+1);
        }
        if (st->next){
            st_string(st->next,space);
        }
    }

}
symbol* find_symbol_functions(symbol_table* cur,string target){
    // check parent only
    while(1){
        if (cur->coll.find(target) == cur->coll.end()){
            cur = cur->parent;
            if (! cur){
                break;
            }
        }else{
            if (cur->coll.find(target)->second->declare_type == FDECLARE){
                return cur->coll[target];
            }
            cur = cur->parent;
            if (!cur){
                break;
            }
        }
    }
    return NULL;
}
symbol* find_symbol_variable(symbol_table* cur,string target){
    cout << "91th\n";
    symbol_table* prev = NULL;
    while(1){
        if (cur->coll.find(target) == cur->coll.end()){
            cur = cur->parent;
            if (! cur){
                break;
            }else{
                // check the global variable and make a record
                // my global include the parameter list, so we check the parameter outer of loop
                if (cur->scope == "Global"){
                    prev = cur;
                }
            }
        }else{
            cout << "44th\n";
            cout << cur->coll.find(target)->second->declare_type << endl;
            if (cur->coll.find(target)->second->declare_type == VDECLARE
            || cur->coll.find(target)->second->declare_type == LVDECLARE) {
                return cur->coll[target];
            }
            cur = cur->parent;
            if (!cur){
                break;
            }
        }
    }
    // check the parameter list of this function
    /**
    if (!prev){
        cout << "119th\n";
        return NULL;
    }
     **/
    cout << function_name << endl;
    parameter* p = prev->coll[function_name]->parameters;
    while(p){
        cout << "59th\n";
        cout << p->id << endl;
        cout << target << endl;
        if (p->id == target){
            // find it
            cout << "found it in 62th\n";
            symbol* rv = new symbol();
            // rv->parameters = prev->coll[function_name]->parameters;
            rv->array = p->array;
            rv->type = p->type;
            collector.push_back(rv);
            return rv;
        }
        p = p->next;
    }
    cout << "138th\n";
    return NULL;
}
int analyze_expression(TreeNode* icode, symbol_table* cur){
    if(icode->op == VAR){
        cout << "65th\n";
        symbol* s = find_symbol_variable(cur,icode->id);
        cout << "67th\n";
        if (!s){
            cerr << "[error " << icode->id << " is undeclared]" << endl;
            exit(1);
        }
        if (icode->array != s->array){
            cout << icode->array << endl;
            cout << s->array << endl;
           //if(!function_call){
                cerr << "[error " << icode->id << " is undeclared (array and not array)" << endl;
                exit(1);
            //}
        }
        if (s->array){
            // check the index is interger
            cout << "103th\n";
            cout << icode->id << endl;
            // todo has expression or not
            if (icode->child[0]){
                if(analyze_expression(icode->child[0],cur)!=INT){
                    cerr << "[error index is not int type]" << endl;
                    exit(1);
                }
            }
            cout << "108th\n";
        }
        // have to be int
        cout << s->type << endl;
        return s->type;
    }else if (icode->op == PLUS || icode->op == SUB || icode->op == TIMES || icode->op == DIV){
        int rv1 = analyze_expression(icode->child[0],cur);
        int rv2 = analyze_expression(icode->child[1],cur);
        if (rv1 != rv2){
            cerr <<  "[error +-*/ with different types]" <<endl;
            exit(1);
        }
        return INT;
    }else if (icode->op == ASSIGN){
        cout << "108th\n";
        if (icode->child[0]->op != VAR){
            cerr << "[error expression can not be assigned]" << endl;
            exit(1);
        }
        int rv1 = analyze_expression(icode->child[0],cur);
        int rv2 = analyze_expression(icode->child[1],cur);
        if (rv1 != rv2){
            cerr <<  "[error var can not be assigned by different types]" <<endl;
            exit(1);
        }
        return rv1;
    }else if (icode->op == CFUNCTION){
        cout << "125th\n";
        symbol * s0 = find_symbol_variable(cur,icode->id);
        if (s0){
            cerr << "error[ "<< icode->id << " is a variable instead of a function]" << endl;
            exit(1);
        }
//        function_call = true;
        symbol * s = find_symbol_functions(cur,icode->id);
        if (!s){
            cerr << "[error " << icode->id << " is undeclared]" << endl;
            exit(1);
        }
        cout << "129th" << icode->op << endl;
        TreeNode* root = icode->child[0];
        parameter* p = s->parameters;
        // call(a) and p(void)
        if(root && p->type == VOID){
            cerr << "[error number of argument is incorrect]" << endl;
            exit(1);
        }
        while(root && p){
            if (p->array){
                cout << "136th\n";
                cout << root->op << endl;
                if(root->op == VAR){
                    // indexing will be an int but you need an array
                    if (root->child[0]){
                        cerr << "[error unexpected type error " << root->id << "[] ]" <<endl;
                        exit(1);
                    }
                    cout << root->id << endl;
                    // if it is not found or not an array
                    symbol* s1 = find_symbol_variable(cur,root->id);
                    if (! s1){
                        cerr << "[error " << root->id << "is undeclared]" << endl;
                        exit(1);
                    }
                    if(!s1->array){
                        cerr << "[error unexpected type error " << root->id << " ]" <<endl;
                        exit(1);
                    }
                }else{
                    // put an int
                    cerr << "[error unexpected type error " << root->id << " ]" << endl;
                    exit(1);
                }
            }else{
                // int can be an expression
                // using a void as return value, but you need an int
                cout << "253th\n";
                int rv = analyze_expression(root,cur);
                if (rv != p->type){
                    cerr << "[error unexpected type error " << root->id << " ]" <<endl;
                    exit(1);
                }
                // since rv return so it must exist we check whether it is a int
                /**
                cout << "178th\n";
                if(root->op == VAR) {
                    symbol *s1 = find_symbol_variable(cur, root->id);
                    // if not indexing for an array, but you need an int
                    if (s1->array && !root->child[0]) {
                        cerr << "[error unexpected type error " << root->id << " ]" << endl;
                        exit(1);
                    }
                }
                 **/
            }
            p = p->next;
            root = root->next;
        }
        if (!root && !p ){
            // skip
        }else{
            //call() and function is void
            if (root == NULL){
                // no argument
                if (p->id == ""){
                    return s->type;
                }
            }
            cerr << "[error number of argument is incorrect]" << endl;
            exit(1);
        }
        //function_call = false;
        return s->type;

    }
    else{
        // default int
        return INT;
    }
}
void analyze_comparison(TreeNode* icode, symbol_table* cur){
    if (icode->op == LESS || icode->op == LESSTHAN
       || icode->op == GREAT || icode->op == GREATHAN
       || icode->op == EQUAL || icode->op == NOTEQUAL){
        analyze_expression(icode->child[0],cur);
        analyze_expression(icode->child[1],cur);
    }else if (icode->op == ME){
        analyze_comparison(icode->child[0],cur);
    }
}
void analyze_statement(TreeNode* icode, symbol_table* cur, string id){
   if (icode->op == WHILE){
        // search for break
        // 77 is magic number since var can not be 77
        cout << "while\n";
        cur->coll.insert(make_pair("77",new symbol()));
        analyze_comparison(icode->child[0],cur);
        if (icode->child[1]){
            analyze_statement(icode->child[1],cur,id + " while-statement");
        }
        delete cur->coll["77"];
        cur->coll.erase("77");
   }else if (icode->op == IF){
       cout << "IF\n";
       analyze_comparison(icode->child[0],cur);
       if (icode->child[1]){
           analyze_statement(icode->child[1],cur,id + " if-statement");
       }if (icode->child[2]){
           analyze_statement(icode->child[2],cur,id + " else-statement");
       }
   } else if (icode->op == BLOCK){
       if (cur->children == NULL){
           cur->children = analyze_block(icode,cur,id,BLOCK);
       }else{
           // same level block
           symbol_table* walker = cur->children;
           while(walker->next){
               walker = walker->next;
           }
           walker->next = analyze_block(icode,cur,id,BLOCK);
       }
   }else if (icode->op == BREAK){
       if (find_symbol_variable(cur,"77") == NULL){
           cerr << "error [break must appear in while statement]" << endl;
           exit(1);
       }
   }else if (icode->op == RETURN || icode->op ==OUT){

       if (icode->child[0]){
           if (icode -> op == RETURN && function_type == VOID){
               cerr << "error [return the wrong type]" << endl;
               exit(1);
           }else {
               analyze_expression(icode->child[0], cur);
           }
       }
   }else {
       cout << "expression\n";
       analyze_expression(icode,cur);
   }
}
symbol_table* analyze_block(TreeNode* icode, symbol_table* parent, string id,int op){
    cout << "201th\n";
    symbol_table * st = new symbol_table();
    st->scope = id;
    parameter* p = NULL;
  // if (op != FDECLARE){
    st->parent = parent;
    if (op == FDECLARE){
         p = parent->coll[id]->parameters;
    }
    // local var in block
    TreeNode* root = icode->child[0];
    while(root){
        if (root->type == VOID){
            cerr << "error [" + root->id  + " is void type]" << endl;
            exit(1);
        }
        if (op == FDECLARE){
            // check the parameter list
            while(p){
                if ( p->id == root->id){
                    cerr << "error [redefinitions " + root->id  +"]" << endl;
                    exit(1);
                }
                p = p->next;
            }
             p = parent->coll[id]->parameters;
        }
        if (st->coll.find(root->id) != st->coll.end()){
                cerr << "error [redefinitions " + root->id  +"]" << endl;
                exit(1);
        }else{
                symbol* s = new symbol();
                s->type = root->type;
                s->declare_type = LVDECLARE;
                s->array = root->array;
                if (s->array){
                    s->size = root->size;
                }
                st->coll.insert(make_pair(root->id,s));
        }


        root = root->next;
    }
    cout << "240th\n";
    root = icode->child[1];
    while(root){
        analyze_statement(root,st,st->scope);
        cout << "245th\n";
        root = root->next;
    }
    return st;
}
void analyze_parameter(TreeNode* icode, symbol* s){
    map<string,parameter*> check;
    parameter *h = NULL, *t = NULL;
    while(icode){
        cout << "278th\n";
        if (icode->id != "" && icode->type == VOID){
            cerr << "error [" + icode->id  + " is void type]" << endl;
            exit(1);
        }
        parameter* p = new parameter();
        p->type = icode->type;
        p->array = icode->array;
        p->id = icode->id;
        if ( check.find(icode->id) != check.end()){
            // if we find the same name and same type
            cerr << "error [redefinition of parameter " + icode->id + "]"<< endl;
            exit(1);
        }
        check.insert(make_pair(icode->id,p));
        if (h ==NULL){
            h = t = p;
        }else{
            t->next = p;
            t = t->next;
        }
        icode = icode -> next;

    }
    // header of the parameter list
    s->parameters = h;
}
symbol_table* analyze(TreeNode* icode) {
    /* analyze all the statements in the program */
    symbol_table* st = new symbol_table();
    st->scope = "Global";
    while(icode){
        if (icode->op == VDECLARE){
            if (icode->type == VOID){
                cerr << "error [" + icode->id  + " is void type]" << endl;
                exit(1);
            }
            if (st->coll.find(icode->id) != st->coll.end()) {
                // if we find the same name and same type
                cerr << "error [redefinition " + icode->id + " in the global scope]" << endl;
                exit(1);
            }
            symbol* s = new symbol();
            s->array = icode->array;
            s->type = icode->type;
            s->declare_type = VDECLARE;
            if (s->array){
                s->size = icode->size;
            }
            st->coll.insert(make_pair(icode->id,s));
        }
        else if (icode->op == FDECLARE){

            if( st->coll.find(icode->id) != st->coll.end() ) {
                cerr << "error [redefinition " + icode->id + " in the global scope]" << endl;
                exit(1);
            }
            symbol* s = new symbol();
            s->type = icode->type;
            s->declare_type = FDECLARE;
            analyze_parameter(icode->child[0],s);
            st->coll.insert(make_pair(icode->id,s));
            function_name = icode->id;
            function_type = icode->type;
            if (st->children == NULL) {
                st->children = analyze_block(icode->child[1], st, icode->id, icode->op);
            }else{
                symbol_table* walker = st->children;
                while(walker->next){
                    walker = walker->next;
                }
                walker->next =  analyze_block(icode->child[1], st, icode->id, icode->op);
            }

        }
        icode = icode ->next;
    }
    return st;
}