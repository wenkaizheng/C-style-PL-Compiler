#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include <string>
#include<vector>

using namespace std;
/* Lexical Analyzer for the calculator language */

/* states in the scanner DFA */
typedef enum {
    START, INNUM, DONE, INID, INLESS, INEUQAL,INNOTEQUAL,INGREAT, INCOMMENTS,INCOMMENT,INCOMMENTE
} StateType;
// if no more match can be found
// if there is a /* but end
int back_wards_pos = 0;
std::string key_word[7] = {"int", "void", "if", "else", "while", "return","break"};
int key_word_class[7]  = {INT,VOID,IF,ELSE,WHILE,RETURN,BREAK};
bool check_key_word(TokenType* token) {
    for (int i = 0; i < 7; i++) {
        if (key_word[i] == token->TokenString) {
            token->TokenClass = key_word_class[i];
            return true;
        }
    }
    return false;
}
bool LayOutCharacter(char c) {
    return ((c == ' ') || (c == '\t') || (c == '\n'));
}

TokenType getToken(void) {
    /* holds current token to be returned */
    TokenType currentToken;
    /* current state - always begins at START */
    StateType state = START;
    /* next character */
    char c;
    /* flag to indicate if c is part of the current token */
    bool putback = false; /* putback == true => no */

    do {
        c = cin.get();
    } while (LayOutCharacter(c));
    // main tokenizer loop: one character at a time
    while (state != DONE) {
        switch (state) {
            int rc;
            case START:
                if (isdigit(c))
                    state = INNUM;
                //todo ask Pro Lu
                else if (isalpha(c)) {
                    state = INID;
                    currentToken.TokenClass = ID;
                } else {
                    state = DONE;
                    switch (c) {
                        case EOF:
                            currentToken.TokenClass = ENDFILE;
                            break;
                        case '+':
                            currentToken.TokenClass = PLUS;
                            break;
                        case '-':
                            currentToken.TokenClass = SUB;
                            break;
                        case '*':
                            currentToken.TokenClass = TIMES;
                            break;
                        case '/':
                            currentToken.TokenClass = DIV;
                            state = INCOMMENTS;
                            break;
                        case '!':
                            currentToken.TokenClass = NOT;
                            //check_next_equal(state);
                            state = INNOTEQUAL;
                            break;
                        case '?':
                            currentToken.TokenClass = QUE;
                            break;
                        case '<':
                            currentToken.TokenClass = LESS;
                            //check_next_equal(state);
                            state = INLESS;
                            break;
                        case '>':
                            currentToken.TokenClass = GREAT;
                            //check_next_equal(state);
                            state = INGREAT;
                            break;
                        case '(':
                            currentToken.TokenClass = LPAREN;
                            break;
                        case ')':
                            currentToken.TokenClass = RPAREN;
                            break;
                        case '=':
                            currentToken.TokenClass = ASSIGN;
                            state = INEUQAL;
                            break;
                        case '$':
                            currentToken.TokenClass = OUT;
                            break;
                        case ';':
                            currentToken.TokenClass = SEMI;
                            break;
                        case ',':
                            currentToken.TokenClass = COM;
                            break;
                        case '{':
                            currentToken.TokenClass = STA;
                            break;
                        case '}':
                            currentToken.TokenClass = END;
                            break;
                        case '[':
                            currentToken.TokenClass = LEFT;
                            break;
                        case  ']':
                            currentToken.TokenClass = RIGHT;
                            break;
                        default:
                            currentToken.TokenClass = ERROR;
                            break;
                    }
                }
                break;
            case INNUM:
                if (!isdigit(c)) { /* backup in the input */
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = NUM;
                }
                break;
            case INID:
                //stop if nor num and letter
                if (!isdigit(c) and !isalpha(c) ){
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = ID;
                }
                break;
            case INEUQAL:
                // ==
                if (c == '='){
                    state = DONE;
                    currentToken.TokenClass = EQUAL;

                }
                // =
                else{
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = ASSIGN;
                }
                break;
            case INLESS:
                // <=
                if (c == '='){
                    state = DONE;
                    currentToken.TokenClass = LESSTHAN;

                }
                    // <
                else{
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = LESS;
                }
                break;
            case INGREAT:
                // >=
                if (c == '='){
                    state = DONE;
                    currentToken.TokenClass = GREATHAN;

                }
                    // >
                else{
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = GREAT;
                }
                break;
            case INNOTEQUAL:
                // !=
                if (c == '='){
                    state = DONE;
                    currentToken.TokenClass = NOTEQUAL;

                }
                    // !
                else{
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = ME;
                }
                break;
            case INCOMMENTS:
                // /*
                if (c == '*'){
                    state = INCOMMENT;
                    currentToken.TokenClass = EMPTY;
                }
                else{
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = DIV;
                };
                break;
            // only for end *
            case INCOMMENT:
                if ( c == '*'){
                    state = INCOMMENTE;
                    currentToken.TokenClass = EMPTY;
                }
                //read whatever into token
                break;
            // */ for end of comment
            case INCOMMENTE:
                if ( c == '/'){
                    state = START;
                    // set to empty to express ignore
                }else{
                    if (c != '*') {
                        state = INCOMMENT;
                    }
                }
                break;
            case DONE:
            default: /* should never happen */
                state = DONE;
                currentToken.TokenClass = ERROR;
                break;
        }
        if (!putback ) { // add c to the TokenString
            currentToken.TokenString += c;
            if (state == START){
                currentToken.TokenString = "";
               // return getToken();
                do {
                    c = cin.get();
                } while (LayOutCharacter(c));
                // because we read this char in line 255
                cin.putback(c);
            }
        }

        if (state != DONE)
            c = cin.get();
    }
    // we need to check any keyword
    check_key_word(&currentToken);
    return currentToken;
} /* end getToken */


