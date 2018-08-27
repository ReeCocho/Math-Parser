# Math Parser
Math Parser is a small little mathematical expression parser I wrote in C to test my language skills. It uses no external libraries other than the standard C library. The parser works in two stages. Lexical analysis and parsing. The lexical analysis stage reads the user's input and breaks it up into a set of tokens. These tokens contain a type (Number, addition, subtraction...) and in the case of a number, a string containing their value. These tokens are fed into the parser. The parser takes the tokens and converts them into [reverse polish notation](https://en.wikipedia.org/wiki/Polish_notation) using the [shunting yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm). These new tokens are fed into the final parser which evaluates the expressions and spits out the results.

## Building
CMake is used for the build system, but you could just as easily compile it directly from the command line since there aren't many files. 

## Usage
After executing the program in the command line, you can enter any standard mathematical expression. For example `11 + (5 - 6) / - 2`. Would result in the output, `11.5`. supported operations are listed below.

### Operations
1. Real number Ex. `1.0`
2. Addition (+)
3. Subtraction (-)
4. Multiplication (*)
5. Division (/)
6. Negation Ex. `-1.0`

## Planned Features
A list of planned features is given below.

1. Exponentiation
2. Variables
3. Standard functions
4. Custom functions
5. Comparisons