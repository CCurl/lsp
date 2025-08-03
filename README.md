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

Running:
```
make tc
make vm
make test
make bm
 ```
It is broken into 2 parts:

tc: the compiler.
- This generates an representation of the program in a stack machine format.
- That output is written to file 'tc.out'.
- It also generates file 'tc.sym', a listing of the symbols defined.

vm-stk: an emulator that can run the output from tc.
- It generates file 'vm-stk.sym', a listing of the symbols defined.

TODO:
- [ ] Create a simple emulator that can run a subset of x86 machine code.
- [ ] Generate x86 machine code for the above emulator.
- [ ] Generate x86 an executable program.
