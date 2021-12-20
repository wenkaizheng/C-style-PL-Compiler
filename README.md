# Create my own complier step by step
 Four main parts are Scanner,Parser(Abstract Syntax Tree),Semantic checker, Code generator

To compile this project:

   make ibm
   
   make


# The current version supports those things:

Keywords: int  void  if  else  while  return  break

Operators: +  -  *  /  =  !  <  >  <=  >=  ==  !=  ?  $

Delimiters: (  )  {  }  [  ]  ;  ,

Identifier: any letter followed by 0 or more alphanumeric characters

Number: a non-negative integer


The LL1 grammars are implemented as below:

program → declarations

declarations → declaration declarations’

declarations’ → declaration declarations’ | ε

declaration → var-declaration | fun-declaration

var-declaration → type ID; | type ID [NUM];

type → int | void

fun-declaration → type ID ( formals ) block

formals → void | parameters

parameters → parameter parameters’

parameters’ → ,parameter parameters | ε

parameter → type ID array

array → [] | ε

block → { local-vars statements }

local-vars → local-vars var-declaration | ε

statements → statement statements | ε

statement → expr-stmt | block | selection | break | repetition | return-stmt | $ expression;

expr-stmt → expression ; | ;

selection → if ( comparison ) statement selection’

selection’ → else statement

repetition → while ( comparison ) statement

return-stmt → return return-stmt’

return-stmt’ → ; | expression ;

break → break ;

expression → var = expression | arith-expr

var → ID | ID [ expression ]

comparison → arith-expr relop arith-expr | ! ( comparion )

relop → <= | < | > | >= | == | !=

arith-expr → arith-term arith-expr’

arith-expr’ → addop arith-term arith-expr’ | ε

arith-term → arith-factor arith-term’

arith-term’ → mulop arith-factor arith-term’ | ε

addop → + | -

multop → * | /

arith-factor → var | call | NUM | (expression) | ?

call → ID (actuals)

actuals → ε | arguments

arguments → argument arguments’

arguments → ,argument arguments’ | ε

argument → expression

# This compiler uses IBM instruction, for tesing it try following command

./test < test_progam> test_program_out

./ibm test_program_out

Tail recursion is depreciated since it has some bug.

The register allocation method I used is Sethi–Ullman algorithm.
