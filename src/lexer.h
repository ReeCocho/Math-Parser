#ifndef MP_LEXER_H
#define MP_LEXER_H

/**
 * Functions which take strings, turns them into tokens, and
 * pumps those tokens into the parser.
 */
 
// Token datatype
typedef struct
{
	// Token ID
	int id;
	
	// Token string
	char* str;
	
} token;

// Token types
#define MP_TOKEN_ERR -1
#define MP_TOKEN_NUM 0
#define MP_TOKEN_ADD 1
#define MP_TOKEN_SUB 2
#define MP_TOKEN_MUL 3
#define MP_TOKEN_DIV 4

/**
 * Function which reads a string, turns it into tokens,
 * and pumps those tokens into the parser.
 * @param String.
 */
extern void mp_lex_string(const char* str);
#endif