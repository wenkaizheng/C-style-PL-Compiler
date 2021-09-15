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
    START, INNUM, DONE, INID, INOP
} StateType;
// if no more match can be found
int back_wards_pos = 0;
std::string key_word[10] = {"int", "void", "if", "else", "while", "return","<=",">=","!=","=="};
int key_word_class[10]  = {INT,VOID,IF,ELSE,WHILE,RETURN,LESSTHAN,GREATHAN,NOTEQUAL,EQUAL};
int key_word_class2[4] = {LESS,GREAT,ME,ASSIGN};
bool check_key_word(TokenType* token) {
    for (int i = 0; i < 6; i++) {
        if (key_word[i] == token->TokenString) {
            token->TokenClass = key_word_class[i];
            return true;
        }
    }
    return false;
}
int identifier(TokenType token, bool single){

    if (token.TokenString.back() == '!'){
        return single? key_word_class2[2]:key_word_class[8] ;
    }else if (token.TokenString.back() == '='){
        return single? key_word_class2[3]:key_word_class[9];
    }else if (token.TokenString.back() == '>'){
        return single? key_word_class2[1]:key_word_class[7];
    }else if (token.TokenString.back() == '<'){
        return single? key_word_class2[0]: key_word_class[6];
    }
    return -1;
}
bool check_next_equal(StateType& state){
    char tmp  = cin.get();
    if (tmp == '='){
        state = START;
    }
    else{
        state = DONE;
    }
    cin.putback(tmp);
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
                            break;
                        // todo single maybe a error
                        case '!':
                            currentToken.TokenClass = NOT;
                            //check_next_equal(state);
                            state = INOP;
                            break;
                        case '?':
                            currentToken.TokenClass = QUE;
                            break;
                        case '<':
                            currentToken.TokenClass = LESS;
                            //check_next_equal(state);
                            state = INOP;
                            break;
                        case '>':
                            currentToken.TokenClass = GREAT;
                            //check_next_equal(state);
                            state = INOP;
                            break;
                        case '(':
                            currentToken.TokenClass = LPAREN;
                            break;
                        case ')':
                            currentToken.TokenClass = RPAREN;
                            break;
                        case '=':
                            currentToken.TokenClass = ASSIGN;
                            state = INOP;
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
            case INOP:
                // == != >= <=
               // rc = identifier(currentToken);

                if (c == '='){
                    rc = identifier(currentToken, false);
                    state = DONE;
                    currentToken.TokenClass = rc;

                }
                else{
                    rc = identifier(currentToken, true);
                    cin.putback(c);
                    putback = true;
                    state = DONE;
                    currentToken.TokenClass = rc;
                }

                break;
            case DONE:
            default: /* should never happen */
                state = DONE;
                currentToken.TokenClass = ERROR;
                break;
        }
        if (!putback) // add c to the TokenString
            currentToken.TokenString += c;

        if (state != DONE)
            c = cin.get();
    }
    // we need to check any keyword
    check_key_word(&currentToken);
    return currentToken;
} /* end getToken */


