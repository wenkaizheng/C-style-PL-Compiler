To compile:

   make

For a clean start, type 'make clean' followed by 'make'.

To run the compiler:

   ./calculator

then enter a sequence of statements (assignments or output). Here is
an example:

   x = 2 + 3;
   $ x + x;
   y = x;
   $ y - 1;

Terminate the input with 'Ctrl-D'. This will print the output on the
terminal.

You may also redirect input and output in the usual way. If you
already have an input file (say 'test_input') with a combination of
assignment and output statements, and if you want the compiled code to
be saved to a file called 'test_output.ibm':

   ./calculator < test_input > test_output.ibm

To test the output program:

   make ibm
   ./ibm test_output.ibm

This will load the assembly program 'test_output.ibm'. To run the
program, type 'g' while inside the ibm simulator. Type 'h' to see the
list of commands that the simulator recognizes.

