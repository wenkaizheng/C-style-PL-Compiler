To compile:

   make

For a clean start, type 'make clean' followed by 'make'.

I changed the file parse.cc and globals.h, and add a file test.cc which include main function for testing, and I also add test.cc to Makefile.

For testing and printing the AST, please tried this.
./test < test_input_file

The following important modified grammars will be implemented:

Declarations -> Declaration Declarations'
Declarations' -> Declaration Declarations' | empty string

Parameters -> Parameter Parameters'
Parameters' -> ,Parameter, Parameters' | empty string

Local-Vars -> Var-Declaration Local-Vars | empty string

Statements -> Statement Statements | empty string

Arith-Expr -> Arith-Term Arith-Expr'
Arith-Expr' -> Addop Arith-Term Arith-Expr' | empty string

Arith-Term -> Arith-Factor Arith-Term'
Arith-Term' -> Mulop Arith-Factor Arith-Term' | empty string


Arguments -> Argument Arguments'
Arguments' -> ,Argument Arguments' | empty string