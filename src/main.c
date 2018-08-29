#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "user_input.h"
#include "lexer.h"
#include "parser.h"

// Entry point
int main(int argc, char* argv[])
{
	// Welcome message
	printf("Welcome to the math parser!\n");
	printf("Say \"exit\" to quit the program\n");
	
	// Initialize the parser
	mp_init_parser();
	
	while(1)
	{
		// Write cursor
		printf("> ");
	
		// Read user input
		char* str = mp_get_user_input();
		
		// Determine if we want to quit
		if(strcmp(str, "exit") == 0) break;
		
		// Lex the input
		mp_lex_string(str);
		
		// Free the user's string
		free(str);
		
		// Parse everything
		mp_parse_all();
	}
	
	// Flush tokens and variables
	mp_flush_parser_tokens();
	mp_flush_variables();
	
	// Quit message
	printf("Closing...");
	
	return 0;
}