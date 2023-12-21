# hw2 report

|||
|-:|:-|
|Name|Tony Chu|
|ID|110xxxxxx|

## How much time did you spend on this project

About 5 hours 😵‍💫.

## Project overview

This is a project for us to practice syntax analysis for P language. \
I used YACC to implement the parser by Context Free Grammar. \
Understand the grammatical structure of P language and how to design its parser.
And I used test cases to think all of the syntax struct of P language may have. \
I write the grammar according to the order of spec first, and then insert some needed grammar.
#### For Example
##### Args 
        Args 
        : 
        | NonEmptyArg
        ;
        NonEmptyArg
        : IDList ':' ScalarType
        ;
(It's needed after writing the grammar of Function Declaration and Definition)
##### IntConst and ScalarType
       IntConst
       : OCT
       | DEC
       ;

       ScalarType
       : INTEGER 
       | BOOLEAN 
       | REAL 
       | STRING
       ;
(They're needed after writing the grammar of Scalar Declaration and Array Type)

## What is the hardest you think in this project

I think the biggest difference between this assignment and HW1 is that in HW1 I can do a part of the test and run `make test` to debug, however, in HW2 I should finish all of it before test it since it is context free grammar. 

I spent too much time dealing with the '\n' at the end of each test file because I forgot to modify the scanner, I just paste scanner directly from my HW1 🤣🤣🤣. \
After modifying the scanner, I encountered some strange problems because I use **BEGIN** as a token, however, this is a reserved word in YACC and I don't know it at first 😢. ( My friends told me that so I used **BEGIN_** instead. )

And I didn't familiar with YACC 😢.

## Feedback to T.A.s

1. I think spec should be more detailed, for example, at the beginning of the spec, it says how to declare a function, 
    ####
          identifier (<zero or more formal arguments, separated by semicolons>): scalar_type;
    but I don't know what is `scalar_type` at first, so I recommand that `scalar type` should be explained immediately after using it.

2. Highlight some important notes. 

    For Example: 

    Change 

    The `identifier_list` contains at least one identifier 

    to 

    The `identifier_list` contains `at least one` identifier 

3. Give some example of each grammer. 

    For Example: 

    In variable declaration:
    ####
            var a: integer;
            var b, e: real;
            var c: string;        
            var arr2d: array 3 of array 4 of real;

The above recommandations are the inconvenience parts when I'm doing my assignment 

Thanks 😃 !