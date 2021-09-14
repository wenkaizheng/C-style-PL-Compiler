#include <iostream>
#include "globals.h"

int main(void) {
    // Just for testing the tokenizer

    TokenType t = getToken();
    while (t.TokenClass != ENDFILE) {
        cout << t.TokenClass << ":" << t.TokenString << endl;
        t = getToken();
    }
    std::cout << "done\n";
    exit(0);
}

