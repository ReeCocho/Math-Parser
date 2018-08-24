#include "stdio.h"
#include "stdlib.h"
#include "user_input.h"
#include "lexer.h"
#include "parser.h"

// Entry point
int main(int argc, char* argv[])
{
	// Initialize the parser
	mp_init_parser();
	
	while(1)
	{
		// Read user input
		char* str = mp_get_user_input();
		
		// Lex the input
		mp_lex_string(str);
		
		// Free the user's string
		free(str);
		
		// Parse everything
		mp_parse_all();
	}
	
	return 0;
}