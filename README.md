This is a simple compiler, disassembler and vm-emulator, mostly for learning.<br/>
It also includes a heap-based memory management system.<br/>
It currently supports a C-like syntax.<br/>
It's primary purpose is to be a pedagogical tool for learning about compilers.<br/>
The compiler does a minimal amount of error checking to help highlight the structure of the compiler.<br/>
<br/>
The grammar of language in EBNF is:

```
  <program>   ::= <defs>
  <defs>      ::= <def> | <def> <def>
  <def>       ::= <func_def> | <var-def>
  <func-def>  ::= "void" <id> "()" "{" <statement> "}" |
  <var-def>   ::= "int" <id> ";"
  <statement> ::= "if" <paren_expr> <statement> |
                  "if" <paren_expr> <statement> "else" <statement> |
                  "while" <paren_expr> <statement> |
                  "do" <statement> "while" <paren_expr> ";" |
                  "{" <statement> "}" |
                  <func-call> ";" |
                  <expr> ";" |
                  <id> "=" <expr> ";" |
                  ";"
  <paren_expr> ::= "(" <expr> ")"
  <expr>       ::= <test> | <math>
  <test>       ::= <math> <test-op> <math>
  <test-op>    ::= "<" | "==" | ">"
  <math>       ::= <term> | <math> <math_op> <term>
  <math-op>    ::= "+" | "-" | "*" | "/"
  <term>       ::= <id> | <int> | <paren_expr>
  <id>         ::= <alpha><alpha-numeric>*
  <int>        ::= [0-9]*
  <func-call>  ::= <id> "(" ")"
 ```
