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
#define MP_TOKEN_NEG 5

#define MP_TOKEN_LPN 6
#define MP_TOKEN_RPN 7

// Token associativity types
#define MP_LEFT_ASSOC 0
#define MP_RIGHT_ASSOC 1

// Token precedence
extern int mp_token_precedence[6];

// Token associativity
extern char mp_token_assoc[6];

/**
 * Function which reads a string, turns it into tokens,
 * and pumps those tokens into the parser.
 * @param String.
 */
extern void mp_lex_string(const char* str);
#endif