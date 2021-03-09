# Cypress

**Cypress** is real early design of a new programming language. This language is being developed not to accomplish any particular task, instead it is mostly an experiment in understanding further the process.

Cypress is built in C++ as an embedded language first, there are plans to make the language an interpreted language once its completed to an acceptable stage with plenty of functionality. At which point the scripts can be interpreted using the binary.

This is a very early version of the project. Currently it is not functional.

Missing:

- Lexer is covering all of planned base language, but there are features missing.
- Most of the AST is not completed.
- AST does not execute. Currently it only outputs the structure of the tree.
- No tests are completed.

The language is planned to be a Go-like language: functional with struct types and with methods attached like go, interfaces, multiple value returns, and error being a returned type rather than try and catch; a C-like language with strict type declaration for each variable in front `int a`; Rust-like in the use of short keyword names, `fn`, and much more.
