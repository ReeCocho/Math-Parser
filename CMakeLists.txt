cmake_minimum_required(VERSION 3.12)

# Project
project("Math-Parser")

# Executable
add_executable (
	"Math-Parser"
	"src/lexer.c"
	"src/lexer.h"
	"src/main.c"
	"src/math_funcs.c"
	"src/math_funcs.h"
	"src/parser.c"
	"src/parser.h"
	"src/user_input.c"
	"src/user_input.h"
)