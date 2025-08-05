This is a simple compiler, disassembler and vm-emulator, mostly for learning.<br/>
It also includes a heap-based memory management system.<br/>
It currently supports a C-like syntax.<br/>
It's primary purpose is to be a pedagogical tool for learning about compilers.<br/>
The compiler does a minimal amount of error checking to help highlight the structure of the compiler.<br/>
<br/>
Note: I am interested in learning how compilers work, and I came across Marc Feeley's Tiny-C.<br/>
I found it here: http://www.iro.umontreal.ca/~felipe/IFT2030-Automne2002/Complements/tinyc.c<br/>
Seeing the copyright, I emailed Mark and asked him if I could use it, and he said<br/>
that it was under a MIT license and I could do what I wanted with it.<br/>
This work is based on Marc's tinyc.c effort.<br/>
<br/>
The idea is to build on tiny-c to generate runnable, program in byte-code.<br/>
That program (tc.out) can be input into gen-lin to create a native executable for Linux.<br/>
<br/>
The grammar of language in EBNF is:

```
  <program>   ::= <defs>
  <defs>      ::= <def> | <def> <def>
  <def>       ::= <func_def> | <int-def> | <byte-def>
  <func-def>  ::= "void" <id> "()" "{" <statement> "}" |
  <int-def>   ::= "int"  <id> ";" | "int" <id>  "[" <int> "]" ";"
  <byte-def>  ::= "byte" <id> ";" | "byte" <id> "[" <int> "]" ";"
  <statement> ::= "if" <paren_expr> <statement> |
                  "if" <paren_expr> <statement> "else" <statement> |
                  "while" <paren_expr> <statement> |
                  "do" <statement> "while" <paren_expr> ";" |
                  "{" <statement> "}" |
                  <func-call> ";" |
                  <expr> ";" |
                  <id> "=" <expr> ";" |
                  <id> "[" <expr> "]" "=" <expr> ";" | (future)
                  "//" |
                  ";"
  <paren_expr> ::= "(" <expr> ")"
  <expr>       ::= <math> | <math> <test-op> <math>
  <test-op>    ::= "<" | "==" | ">"
  <math>       ::= <term> | <math> <math_op> <term>
  <math-op>    ::= "+" | "-" | "*" | "/"
  <term>       ::= <id> | <int> | <paren_expr>
  <id>         ::= <alpha><alpha-numeric>*
  <int>        ::= [0-9]*
  <func-call>  ::= <id> "(" ")"
 ```

It is broken into multiple parts
- each part is implemented in a single file.

tc.c: the tiny-c compiler
- This takes a .tc file as the only argument.
- If no argument is given, it reads the source from stdin.
- The output is written to file 'tc.out'.
- tc.out is a representation of the program in a stack machine format.
- It also generates file 'tc.sym', a listing of the symbols defined.

hex-dump.c: A little program to dump a file's contents in hex.

vm-stk.c: an emulator
- This is a stack machine VM emulator that can run the output from tc.
- It generates file 'vm-stk.lst', a disassembly listing of the program.

gen-lin.c: generates a native Linux executable from tc.out
- currently working on it
- input comes from stdin
- output goes to stdout
- messages are written to stderr

Running:
```
make tc
make vm-stk
make gen-lin
make test
make lin-test
make bm
make lin-bm
 ```

TODO:
- [ ] Create a simple emulator that can run a subset of x86 machine code.
- [ ] Generate x86 machine code for the above emulator.
- [ ] Generate x86 an executable program.
