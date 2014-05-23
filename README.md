XY Compiler
============================

* The code for the compiler is in `/src`
* The code for the C runtime libraries to be used with the compiled output is in `/clib`
* The [XY interpreter](https://github.com/iitalics/XY-lang) used to build this project can be obtained on my github

This project is nowhere close to be completed, much of the C library still needs to be completed
and the compiler itself is still missing many important features

Compiler Features To Be Implemented
----------------
* lazy-evaluated `and` and `or` expressions
* `&OPERATOR` lambdas
* `with` expressions
* lists
* list comprehension (`$` syntax)
* maps
* `use` file inclusion
* the entire standard library