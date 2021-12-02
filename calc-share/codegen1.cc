#include <iostream>
#include "globals.h"
#include<list>
#include<map>
/* Code Generation for IBM */
symbol_table* code_generation_block(TreeNode* root, symbol_table* parent, string id,bool function);
static CodeType codeArray[CODESIZE];
static int ICounter = 0;
static int C_OFFSET = 997; // the *fixed* address assigned to variable 'a'
static int tempAddr = 100;
static symbol_table* global_st = NULL ;
static symbol_table* prev_st = NULL ;
static string lda_or_ld = "LD";
static int global_offset;
static int not_counter = 0;
static bool if_flag = false;
static bool else_flag = false;
static bool first = true;
// we save the number of ICounter and used for adding back patching
list<pair<string,int> > gap_array;
void emit(string code, OpCodeType ctype,
	  int operand1, int operand2, int operand3, int index = -1) {
    int pos = -1;
   // cout << "39th\n";
    if (index == -1) {
        pos = ICounter;
        ICounter += 1;
    }else{
       pos =  index;
    }
   // cout << code <<" " ;
  //  cout << pos <<  code << endl;
    codeArray[pos].opcode = code;
    codeArray[pos].ctype = ctype;
    codeArray[pos].rand1 = operand1;
    codeArray[pos].rand2 = operand2;
    codeArray[pos].rand3 = operand3;
   // cout << "55th\n";

}

int get_local_offset(symbol_table* st, string id, int& v){
   // cout << "59th\n";
   // cout << global_st->coll.size() << endl;
    while(1){
        if (st->scope == "Global"){
            return false;
        }
        if (st->scope != "Global" && st->coll.find(id) != st->coll.end()){
           // cout << "47th\n";
           v = st->coll[id]->offset;
         //   cout << v << endl;
           return true;
        }
        st = st->parent;
    }
}

int get_parameter_offset(string func_name, string para_name, int& v){
  //  cout << "71th\n";
  //  cout << func_name << endl;
  //  cout << global_st->coll.size() << endl;
   // cout << "76th\n";
    if (global_st->coll.find(func_name) == global_st->coll.end()){
        cout << "77th\n";
    }
    parameter* p = global_st->coll[func_name]->parameters;
   // cout << "80th\n";
    int rv = 0;
    while(p){
        cout << para_name << endl;
        cout << p->id << endl;
        if (p->id == para_name){
            v = p->offset;
            return true;
        }
        p = p -> next;
    }
    return false;
}
bool get_global_offset(string id, int& v ){
 //   cout << id <<endl;
  //  cout << "93th " << global_st->coll.size() << endl;
   // printf("95th %p\n",&global_st->coll);
    for(auto it = global_st->coll.cbegin(); it != global_st->coll.cend(); ++it)
    {
        std::cout << it->first << "\n";
    }
    if (global_st->coll.find(id) == global_st->coll.end()){
      //  cout << "95th\n";
        return false;
    }else{
      //  cout << global_st->coll.find(id)->second->offset << endl;
        v = global_st->coll[id]->offset;;
        return true;
    }
}
void code_generation_expression(TreeNode* root, symbol_table* cur, int start_register) {
   // cout << "82th\n";
   // cout << global_st->coll.size() << endl;
    //printf("%p\n",&global_st->coll);
    if (root == NULL){
        cout << "103th\n";
       return;
    }
  //  cout << root->op << endl;
     //todo var and functinon call, assign , +-*/
     int used_r1 = -1;
     int used_r2 = -1;
     if (start_register == 0){
         used_r1 = R0;
         used_r2 = R1;
     }else if (start_register == 1){
         used_r1 = R1;
         used_r2 = R2;
     }else if (start_register >= 2) {
             used_r1 = R2;
             used_r2 = -1;
     }
     if (root->op == VAR){
        // cout << "97th\n";
         int offset_rv,offset_rv1,offset_rv2;
         bool rv,rv1,rv2;
       //  cout << root->id << endl;
         rv = get_local_offset(cur,root->id,offset_rv);
       //  cout<< global_st->coll.size()<<endl;
       //  cout << rv << endl;
       //  cout << "107th\n";
         rv1 = get_parameter_offset(cur->scope,root->id,offset_rv1);
        // cout << "109th\n";
         rv2 = get_global_offset(root->id,offset_rv2);
         //cout<< "111th\n";
         int number_parameters = global_st->coll[cur->scope]->number_parameters;
         if (root->array){
             code_generation_expression(root->child[0],cur,start_register);
             //cout << "129th\n";
             // find the address of element in array and load it into r0
             if (!rv){
                 if (rv1){
                     emit("LD",RM,used_r2,-1 - number_parameters + offset_rv1,FP);
                     emit("ADD", RO, used_r1, used_r2, used_r1);
                     emit("LD", RM, used_r1, 0, used_r1);

                 }else{
                     // use sub for global
                     emit("LDA", RM, used_r2, 0 - offset_rv2, GP);
                     emit("SUB", RO, used_r1, used_r2, used_r1);
                     emit("LD", RM, used_r1, 0, used_r1);
                 }
             }else{
                 // local var
                 emit("LDA", RM, used_r2, 1 + offset_rv, FP);
                 emit("ADD", RO, used_r1, used_r2, used_r1);
                 emit("LD", RM, used_r1, 0, used_r1);
             }
         }else{
             if (!rv){
                 if (rv1){
                     emit(lda_or_ld,RM,used_r1,-1 - number_parameters + offset_rv1,FP);
                     lda_or_ld = "LD";
                 }else{
                   //  cout << "144th\n";
                     emit(lda_or_ld,RM,used_r1,0 - offset_rv2,GP);
                     lda_or_ld = "LD";
                 }
             }else{
               //  cout << "162th\n";
                 emit(lda_or_ld,RM,used_r1,1 + offset_rv,FP);
                 lda_or_ld = "LD";
             }
         }
     }else if (root->op == ASSIGN){
        // cout << root->child[0]->id << endl;
         int offset_rv,offset_rv1,offset_rv2;
         bool rv,rv1,rv2;
         rv = get_local_offset(cur,root->child[0]->id,offset_rv);
       //  cout << rv << " " << offset_rv << " 137th\n";
         rv1 = get_parameter_offset(cur->scope,root->child[0]->id,offset_rv1);
       //  cout << rv1<< " " << offset_rv1 << " 139th\n";
         rv2 = get_global_offset(root->child[0]->id,offset_rv2);
      //   cout << rv2 << " " << offset_rv2 << "141th\n";
         if (root->child[0]->array){
             code_generation_expression(root->child[0]->child[0],cur,start_register);
           //  cout << "169th\n";
             if (!rv){
                 if (rv1){
                     //para
                    // FP - 1 - (number of parameters - offset)
                     int number_parameters = global_st->coll[cur->scope]->number_parameters;
                     emit("LD",RM,used_r2,-1 - number_parameters + offset_rv1,FP);
                     emit("ADD", RO, used_r1,used_r2, used_r1);
                     emit("ST",RM,used_r1,0,SP);
                     emit("LDA", RM, SP, 1, SP);
                     // right side value
                     code_generation_expression(root->child[1],cur,start_register);
                     emit("LD", RM, used_r2, -1, SP);
                     emit("LDA", RM, SP, -1, SP);
                     emit("ST", RM, used_r1, 0, used_r2);

                 }else{
                     emit("LDA",RM,used_r2,0-offset_rv2,GP);
                     emit("SUB", RO, used_r1,used_r2, used_r1);
                     emit("ST",RM,used_r1,0,SP);
                     emit("LDA", RM, SP, 1, SP);
                     // right side value
                     code_generation_expression(root->child[1],cur,start_register);
                     emit("LD", RM, used_r2, -1, SP);
                     emit("LDA", RM, SP, -1, SP);
                     emit("ST", RM, used_r1, 0, used_r2);

                 }
             }else{
                 emit("LDA",RM,used_r2,1 + offset_rv,FP);
                 emit("ADD", RO, used_r1,used_r2, used_r1);
                 emit("ST",RM,used_r1,0,SP);
                 emit("LDA", RM, SP, 1, SP);
                 // right side value
                 code_generation_expression(root->child[1],cur,start_register);
                 emit("LD", RM, used_r2, -1, SP);
                 emit("LDA", RM, SP, -1, SP);
                 emit("ST", RM, used_r1, 0, used_r2);

             }

         }else{
            if (!rv){
                if (rv1){
                    // para
                    code_generation_expression(root->child[1],cur,start_register);
                    emit("ST", RM, used_r1, offset_rv1, FP);
                }else{
                    // global
                    code_generation_expression(root->child[1],cur,start_register);
                    emit("ST", RM, used_r1, 0-offset_rv2, GP);
                }
            }else{
                code_generation_expression(root->child[1],cur,start_register);
                emit("ST", RM, used_r1, 1 + offset_rv,FP);//assign: put val in R0 to dMem[val in R1];
            }
         }
         // we need to calculate the right side

     }else if(root->op == PLUS || root->op == SUB || root->op == TIMES || root->op ==DIV){
         if (root->left){
             code_generation_expression(root->child[0], cur, start_register);
             if (root->weight + start_register > 3) {
                 cout << "237th\n";
                 emit("ST", RM, used_r1, 0, SP);
                 emit("LDA", RM, SP, 1, SP);
                 code_generation_expression(root->child[1], cur, start_register);
             }else{
                 cout << root->weight << endl;
                 cout << start_register + 1 << " 248th\n";
                 code_generation_expression(root->child[1], cur, start_register+1);
             }

         }else{
             cout << "255th  " << root->child[1]->id << endl;
             code_generation_expression(root->child[1], cur, start_register);
             if (root->weight + start_register > 3){
                 // store r1 and let next used r1 and r2
                 cout << root->weight << endl;
                 cout << start_register + 1 << " 258th\n";
                 emit("ST", RM, used_r1, 0, SP);
                 emit("LDA", RM, SP, 1, SP);
                 code_generation_expression(root->child[0], cur, start_register);
             }
             else {
                 cout << root->weight << endl;
                 cout << start_register + 1 << " 262th\n";
                 cout << root->child[0]->op << endl;
                 code_generation_expression(root->child[0], cur, start_register + 1);
               //  cout << "253th\n";
             }
         }
         //restore the value in sp
         if (root->weight + start_register > 3 ){
             emit("LD", RM, used_r2, -1, SP);//R1
             emit("LDA", RM, SP, -1, SP);//SP--
         }

         if(root->op == PLUS){
             emit("ADD", RO, used_r1, used_r2, used_r1);
         }else if(root->op == SUB){
             if (root->weight + start_register > 3 ){
                 emit("SUB", RO, used_r1, used_r2, used_r1);
             }
             else {
                 emit("SUB", RO, used_r1, used_r1, used_r2);
             }
         }else if(root->op == TIMES){
             emit("MUL", RO, used_r1, used_r2, used_r1);
         }else{
             if (root->weight + start_register > 3 ){
                 emit("DIV", RO, used_r1, used_r2, used_r1);
             }else {
                 emit("DIV", RO, used_r1, used_r1, used_r2);
             }
         }

     }else if (root->op == CFUNCTION){
         TreeNode* p = root->child[0];
         // we load the parameters into stack
         parameter* argu =  prev_st->coll[root->id]->parameters;
         while(p){
             if (argu->array){
                 lda_or_ld = "LDA";
             }
             code_generation_expression(p,cur,start_register);
             emit("ST", RM, used_r1, 0, SP);
             emit("LDA", RM, SP, 1, SP);
             p = p->next;
             argu = argu->next;
         }

         if (root->tail_recursion){
             int start = global_st->coll[root->id]->start;//find the beginning instruction ICounter
             int number_parameters = global_st->coll[root->id]->number_parameters;
             // fp - number_parameters is the position of the first parameter

             int copy_number_parameters = number_parameters;
             int offset = number_parameters - 1;
             while(number_parameters){
                 emit("LD", RM, used_r1, -1, SP);
                 emit("LDA", RM, SP, -1, SP);
               //  -1 - number_parameters + offset_rv1
                 emit("ST",RM,used_r1, -1 - copy_number_parameters + offset,FP);
                 number_parameters -=1;
                 offset -= 1;
             }
             // save all things in free space and then copy it into original place

             emit("LDC", RM, PC, start, ZERO);//give control to callee which means beginning
            // emit("LD", RM, FP, -1, FP);
             cout << "313th\n";

         }else{

             // push the fp to the stack
             emit("ST", RM, FP, 0, SP);
             emit("LDA", RM, SP, 1, SP);
             // return address
             emit("LDA", RM, used_r1, 3, PC);

             //+3 push return address to the stack
             emit("ST", RM, used_r1, 0, SP);
             emit("LDA", RM, SP, 1, SP);

             int start = global_st->coll[root->id]->start;//find the beginning instruction ICounter
             // we need to + 1 if there is another function

             emit("LDC", RM, PC, start, ZERO);//give control to callee

             emit("LD", RM, FP, -1, FP);//reset FP to callee
             cout << "351th\n";

        }
     }
     else if(root->op == NUM){//number
        // cout << "325th\n";
       //  cout << root->val << endl;
         emit("LDC", RM, used_r1, root->val, used_r1);
        // cout << "299th\n";
        // cout << global_st->coll.size() << endl;

     }else if(root->op == QUE){//user input
         emit("IN", RO, R0, R0, R0);
     }
}
void convert_comparison(int& op){
    if (not_counter % 2 == 1){
        if (op == LESS){
            op = GREATHAN;
        }else if (op ==LESSTHAN){
            op = GREAT;
        }else if (op == GREAT){
            op = LESSTHAN;
        }else if (op == GREATHAN){
            op = LESS;
        }else if (op == EQUAL){
            op = NOTEQUAL;
        }else if (op == NOTEQUAL){
            op = EQUAL;
        }
    }else{
        // do not change anything
    }
    not_counter = 0;
}
int code_generation_comparison(TreeNode* root, symbol_table* cur, int start_register){
    if(root->op == ME){
        not_counter += 1;
        return code_generation_comparison(root->child[0], cur,start_register);
    }else if(root->op == LESS || root->op == LESSTHAN
             || root->op == GREAT || root->op == GREATHAN
             || root->op == EQUAL || root->op == NOTEQUAL){
        // save it in R0 as tmp
        code_generation_expression(root->child[0], cur,start_register);
        emit("ST", RM, R0, 0, SP);
        emit("LDA", RM, SP, 1, SP);

        code_generation_expression(root->child[1], cur,start_register);

        emit("LD", RM, R1, -1, SP);//R1:left
        emit("LDA", RM, SP, -1, SP);//SP--

        emit("SUB", RO, R0, R1, R0);//R0 = R1 - R0
        cout << "371th " << root->op << endl;
        return root->op;
    }
}
void code_generation_statement(TreeNode* root,  symbol_table* cur, string id){
    if (root->op == WHILE){
        int while_true = ICounter;
        int op = code_generation_comparison(root->child[0],cur,0);
        cout << "379th\n";
        not_counter = 0;
        convert_comparison(op);
        cout << "382th\n";
        int while_false = ICounter;
        ICounter += 1;
        if (root->child[1]){
            cout << "386th\n";
            cout << root->child[1]->op << endl;
            code_generation_statement(root->child[1],cur,id);
        }
        cout << "388th\n";
        emit("LDC",RM,PC,while_true,R0);
        if (op == LESS){
            emit("JGE",RM,R0,ICounter,ZERO,while_false);
        }else if (op == LESSTHAN){
            emit("JGT",RM,R0,ICounter,ZERO,while_false);
        }else if (op == GREAT){
            emit("JLE",RM,R0,ICounter,ZERO,while_false);
        }else if (op == GREATHAN){
            emit("JLT",RM,R0,ICounter,ZERO,while_false);
        }else if (op == EQUAL){
            emit("JNE",RM,R0,ICounter,ZERO,while_false);
        }else if (op == NOTEQUAL){
            emit("JEQ",RM,R0,ICounter,ZERO,while_false);
        }
        // todo we need to jump the btk pointer to here
        // icounter -1， pop the gap array and change the break position
        if (gap_array.size() != 0){
            pair<string,int> p = gap_array.front();
            if (p.first == "if"){
                emit("LDC",RM,PC,ICounter,0,p.second);
                if_flag = false;
            }else{
                codeArray[p.second].rand2 = ICounter ;
                else_flag = false;
            }
        }
       // btk = true;
    }else if (root->op == IF){
        int op = code_generation_comparison(root->child[0],cur,0);
        not_counter = 0;
        convert_comparison(op);
        int if_index = -1;
        if (root->child[1]){
            if_flag = true;
            else_flag = false;
            int else_index = ICounter;
            ICounter += 1;
            code_generation_statement(root->child[1],cur,id);
            if_index = ICounter;
            int else_true = ICounter;
            if (root->child[2]){
                else_true += 1;
            }
            // jump to else
            if (op == LESS){
                emit("JGE",RM,R0,else_true,ZERO,else_index);
            }else if (op == LESSTHAN){
                emit("JGT",RM,R0,else_true,ZERO,else_index);
            }else if (op == GREAT){
                emit("JLE",RM,R0,else_true,ZERO,else_index);
            }else if (op == GREATHAN){
                emit("JLT",RM,R0,else_true,ZERO,else_index);
            }else if (op == EQUAL){
                emit("JNE",RM,R0,else_true,ZERO,else_index);
            }else if (op == NOTEQUAL){
                emit("JEQ",RM,R0,else_true,ZERO,else_index);
            }
        }
        if (root->child[2]){
            if_flag = false;
            else_flag = true;
            ICounter += 1;
            code_generation_statement(root->child[2],cur,id);
            emit("LDC", RM, PC, ICounter, R0, if_index);
        }else{
            emit("LDC", RM, PC, ICounter, R0, if_index);
        }
    }else if(root->op == BLOCK){
        cout << "444th\n";
        if(cur->children == NULL){
            cout << "445th\n";
            cur->children = code_generation_block(root,cur,id, false);
        }else{
            symbol_table* walker = cur->children;
            while(walker->next){
                walker = walker->next;
            }
            walker->next = code_generation_block(root,cur,id, false);
        }
    }else if (root->op == RETURN || root->op == OUT){
        code_generation_expression(root->child[0],cur,0);
        cout << "437th\n";
        if (root->op == OUT){
            emit("OUT", RO, R0, R0, R0);
        }
    }else if(root->op == BREAK){
        //todo we need to jump out of the while loop
        // Icounter -1 is for the break case in else block
        if (!if_flag == !else_flag){
            gap_array.push_back(make_pair("if",ICounter));
            ICounter += 1;
        }
        if (if_flag){
            gap_array.push_back(make_pair("if",ICounter));
            ICounter += 1;
        }
        if (else_flag){
            gap_array.push_back(make_pair("else",ICounter-1));
        }

    }else{
       // cout << "462th\n";
        code_generation_expression(root,cur,0);
    };
}
symbol_table* code_generation_block(TreeNode* root, symbol_table* parent, string id, bool func){
    symbol_table* st = new symbol_table();
    st->offset = parent->offset;
    st->scope = id;
    st->parent = parent;
    TreeNode* t = root->child[0];
    //cout << "329th\n";
    //cout << t->id << endl;
    while (t){
        symbol* s = new symbol();
        s->type = t->type;
        s->declare_type = LVDECLARE;
        s->array = t->array;
        s->offset = st->offset;
        if (t->array){
            s->size = t->size;
            st->offset += t->size;
            emit("LDA", RM, SP, t->size, SP);
        }else{
            st->offset++;
            emit("LDA", RM, SP, 1, SP);
        }
        st->coll.insert(make_pair(t->id,s));
       // cout << t->id;
       // printf("478th %p\n",&st->coll);
        t = t->next;
    }
   /// cout << "347th\n";
    t = root->child[1];
    while(t){
        code_generation_statement(t,st,st->scope);
        t = t->next;
      //  cout << "428th\n";
    }
   // cout << "353th\n";
    if(st->scope != "main" && func){
        cout << "559th\n";
        cout << st->scope << endl;
        int number_parameters = global_st->coll[st->scope]->number_parameters;
      //  cout << "356th\n";
        // sp to up
        emit("LDA", RM, SP, -1 - number_parameters, FP);
        // pc is the first fp
        emit("LD", RM, PC, 0, FP);
        //emit("LD", RM,FP, -1, FP);
    }
    return st;

}
void code_generation_parameters(TreeNode* root, symbol* s){
      int number_of_parameters = 0;
      parameter* h = NULL, *t = NULL;
      while(root){
          parameter* p = new parameter();
          p->type = root->type;
          p->offset = number_of_parameters;
          p->array = root->array;
          p->id = root->id;
          number_of_parameters += 1;
          if (h == NULL){
              h = t = p;
          }else{
              t->next = p;
              t = t->next;
          }
          root = root->next;
      }
      s->parameters = h;
      s->number_parameters = number_of_parameters;


}
void code_generation(TreeNode* root,symbol_table* prev){
     prev_st = prev;
     symbol_table* st = new symbol_table();
     st->scope = "Global";
     global_st = st;
     // todo not sure and check with Lu
     st->offset = 0;
     cout << "393th\n";
     emit("LD",RM,GP,0,3);
     while(root){
        if (root->op == VDECLARE){
            symbol* s = new symbol();
            s->array = root->array;
            s->type = root->type;
            s->declare_type = VDECLARE;
            s->offset = global_offset;
           // if (first_global){
             //   first_global = false;
               // emit("LDC",RM,GP,1023,R0,0);
            //}
            if (root->array){
                s->size = root->size;
                global_offset += root->size;
            }else{
                global_offset += 1;
            }
            st->coll.insert(make_pair(root->id,s));
           // emit("LDA",RM,GP,0-s->offset,GP);
        }else if (root->op == FDECLARE){
            symbol* s = new symbol();
            s->type = root->type;
            s->declare_type = FDECLARE;
            s->start = ICounter;
            if(root->id == "main"){
               // int record_icounter = ICounter;
               if(ICounter == 1) {
                   emit("LDC", RM, FP, 1, R0);
                   emit("LDC", RM, SP, 2, R0);
                   emit("LDC", RM, PC, ICounter + 1, R0);
               }else{
                   int record_icounter = ICounter;
                   emit("LDC", RM, FP, 1, R0);
                   emit("LDC", RM, SP, 2, R0);
                   emit("LDC", RM, PC, record_icounter, R0,1);
               }

            }else{
              //  cout << "425th\n";
                if (first){
                    ICounter = 2;
                    s->start = 2;
                    first = false;
                }
            //    emit("ST",RM,FP,0,SP);
                emit("LDA", RM, FP, -1, SP);
               // emit("LDA",RM,SP,1,SP);
              //  cout << "429th\n";
            }
            code_generation_parameters(root->child[0],s);
            st->coll.insert(make_pair(root->id,s));
          //  cout << root->id << endl;
           // cout << "432th\n";
            if (st->children == NULL) {
            //    cout << "434th\n";
                st->children = code_generation_block(root->child[1], st, root->id, true);
             //   cout << "436th\n";
            }else{
                symbol_table* walker = st->children;
                while(walker->next){
                    walker = walker->next;
                }
                walker->next =  code_generation_block(root->child[1], st, root->id,true);
            }
        }
        root = root->next;
     }
}

void print_code(void) {
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