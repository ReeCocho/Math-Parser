/** Includes. */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lexer.h"

int mp_token_precedence[6] =
{
	1,	// Number
	2,	// Add
	2,	// Subtract
	3,	// Multiply
	3,	// Divide
	4	// Negation
};

char mp_token_assoc[6] =
{
	MP_LEFT_ASSOC,	// Number
	MP_LEFT_ASSOC,  // Add
	MP_LEFT_ASSOC,  // Subtract
	MP_LEFT_ASSOC,  // Multiply
	MP_LEFT_ASSOC,  // Divide
	MP_RIGHT_ASSOC  // Negation
};

/** Number of tokens to allocate at a time. */
#define MP_TOKEN_CHUNK_SIZE 8

/**
 * Structure used when converting token's to polish notation.
 */
typedef struct
{
	/** Base token. */
	token t;
	
	/** General purpose flag. */
	size_t flag;
	
} pn_token;

/** Token queue. */
static struct
{
	/** Pointer to array of token objects. */
	token* tokens;
	
	/** Number of tokens. */
	size_t len;
	
	/** Number of tokens allocated. */
	size_t allocated;
	
} token_queue;



void mp_init_parser()
{
	token_queue.tokens = malloc(sizeof(token) * MP_TOKEN_CHUNK_SIZE);
	token_queue.len = 0;
	token_queue.allocated = MP_TOKEN_CHUNK_SIZE;
}

void mp_add_token_to_parser(token t)
{
	// Add token to queue
	token_queue.tokens[token_queue.len++] = t;
	
	// Resize token queue if needed
	if(token_queue.len == token_queue.allocated)
	{
		// Update token allocation counter
		token_queue.allocated += MP_TOKEN_CHUNK_SIZE;
		
		token_queue.tokens = realloc(
			token_queue.tokens, 
			sizeof(token) * token_queue.allocated
		);
	}
}

void mp_flush_parser_tokens()
{
	// Loop over every token
	for(size_t i = 0; i < token_queue.len; ++i)
		// If the token is a number...
		if(token_queue.tokens[i].id == MP_TOKEN_NUM)
			// Free the string
			free(token_queue.tokens[i].str);

	// Free old token queue
	free(token_queue.tokens);
	
	// Reinitialize the parser
	mp_init_parser();
}

/**
 * Convert the current token list into polish notation using the shunting yard algorithm.
 */
static void mp_to_polish_notation()
{
	// Token queue and size
	pn_token* pn_tokens = malloc(sizeof(pn_token));
	size_t pn_len = 0;
	
	// Operator token stack and size
	pn_token* op_tokens = malloc(sizeof(pn_token));
	size_t op_len = 0;
	
	// Next number or paren is negative flag
	char next_is_neg = 0;
	
	// Loop over every token in the original token list
	for(size_t i = 0; i < token_queue.len; ++i)
	{
		// Get current token ID (For convenience)
		const int tok = token_queue.tokens[i].id;
		
		// Number
		if(tok == MP_TOKEN_NUM)
		{
			// Make the value negative if needed
			char* str = token_queue.tokens[i].str;
			if(next_is_neg)
			{
				// Get string length
				size_t len = strlen(str);
				
				// Resize string to fit negative sign
				str = realloc(str, len + 2);
				
				// Shift data in string over by 1 byte
				memmove(str + 1, str, len + 1);
				
				// Add negative sign
				str[0] = '-';
			}
		
			// Add token to queue
			pn_tokens[pn_len].t.id = MP_TOKEN_NUM;
			pn_tokens[pn_len++].t.str = str;			
			pn_tokens = realloc(pn_tokens, sizeof(pn_token) * (pn_len + 1));
			
			// Reset flag
			next_is_neg = 0;
		}
		
		// Left paren
		else if(tok == MP_TOKEN_LPN)
		{			
			// Add 0 to stack if negative (To make the expression 0 - x)
			if(next_is_neg)
			{
				pn_tokens[pn_len].flag = 0;
				pn_tokens[pn_len].t.id = MP_TOKEN_NUM;
				pn_tokens[pn_len].t.str = malloc(2);
				pn_tokens[pn_len].t.str[0] = '0';
				pn_tokens[pn_len++].t.str[1] = '\0';
				pn_tokens = realloc(pn_tokens, sizeof(pn_token) * (pn_len + 1));
			}
	
			// Add to operator stack
			op_tokens[op_len].flag = next_is_neg;
			op_tokens[op_len++].t = token_queue.tokens[i];			
			op_tokens = realloc(op_tokens, sizeof(pn_token) * (op_len + 1));
			
			// Reset flag
			next_is_neg = 0;
		}
		
		// Right paren
		else if(tok == MP_TOKEN_RPN)
		{
			// Pop operators off the stack that aren't left parenthesis
			while(op_len != 0 && op_tokens[op_len - 1].t.id != MP_TOKEN_LPN)
			{
				pn_tokens[pn_len++] = op_tokens[--op_len];
				pn_tokens = realloc(pn_tokens, sizeof(pn_token) * (pn_len + 1));
				op_tokens = realloc(op_tokens, sizeof(pn_token) * (op_len + 1));
			}
						
			// Make negative if needed
			if(op_tokens[op_len - 1].flag == 1)
			{
				// Add a subtraction token to the end
				pn_tokens[pn_len].t.id = MP_TOKEN_SUB;
				pn_tokens[pn_len].t.str = NULL;
				pn_tokens[pn_len++].flag = 0;
				pn_tokens = realloc(pn_tokens, sizeof(pn_token) * (pn_len + 1));
				
				// // Update token stack size
				// pn_tokens = realloc(pn_tokens, sizeof(pn_token) * (pn_len + 3));
				// memmove(pn_tokens + 1, pn_tokens, sizeof(pn_token) * pn_len);
				// 
				// // Update token length
				// pn_len += 2;
				// 
				// // First token is a zero
				// pn_tokens[0].flag = 0;
				// pn_tokens[0].t.id = MP_TOKEN_NUM;
				// pn_tokens[0].t.str = malloc(2);
				// pn_tokens[0].t.str[0] = '0';
				// pn_tokens[0].t.str[1] = '\0';
				// 
				// // Last token is subtraction operator
				// pn_tokens[pn_len - 1].flag = 0;
				// pn_tokens[pn_len - 1].t.id = MP_TOKEN_SUB;
				// pn_tokens[pn_len - 1].t.str = NULL;
			}
			
			// Pop left bracket
			op_tokens = realloc(op_tokens, sizeof(pn_token) * (op_len--));
		}
		
		// Negation operator
		else if (tok == MP_TOKEN_NEG)
		{
			// Set flag
			next_is_neg = 1;
		}
		
		// Must be an operator
		else
		{
			// Pop operator tokens if needed
			if(op_len != 0)
			{
				// Operator token for convenience
				int op_tok = op_tokens[op_len - 1].t.id;
			
				// Loop from the top of the stack
				while(
					// Stack must not be empty
					op_len != 0 &&
					(
						// Precedence is greater
						mp_token_precedence[op_tok] > mp_token_precedence[tok]  ||
						// Precedence is equal, but is left associative
						(
							mp_token_precedence[op_tok] == mp_token_precedence[tok] && 
							mp_token_assoc[op_tok] == MP_LEFT_ASSOC
						) &&
						// Not a left bracket
						op_tok != MP_TOKEN_LPN
					)
				)
				{	
					// Pop operator off the stack 
					pn_tokens[pn_len++] = op_tokens[op_len - 1];
					pn_tokens = realloc(pn_tokens, sizeof(pn_token) * (pn_len + 1));
					
					// Update operator stack length and operator token
					if(--op_len != 0) op_tok = op_tokens[op_len - 1].t.id;
				}
			}
			
			// Push operator onto the stack
			op_tokens[op_len++].t = token_queue.tokens[i];
			op_tokens = realloc(op_tokens, sizeof(pn_token) * (op_len + 1));
			
			// Reset flag
			next_is_neg = 0;
		}
	}
	
	// Add remaining operators onto the stack
	if(op_len != 0) for(size_t i = op_len - 1;; --i)
	{
		// Pop operator off the stack 
		pn_tokens[pn_len++] = op_tokens[i];
		pn_tokens = realloc(pn_tokens, sizeof(pn_token) * (pn_len + 1));
	
		if(i == 0) break;
	}
	
	// Free original tokens
	free(token_queue.tokens);
	
	// Convert pn_tokens to regular tokens
	token_queue.tokens = malloc(sizeof(token) * pn_len);
	token_queue.len = pn_len;
	for(size_t i = 0; i < pn_len; ++i)
		token_queue.tokens[i] = pn_tokens[i].t;
	
	// Free stacks
	free(pn_tokens);
	free(op_tokens);
	
	// // Print loop for debugging
	// for(size_t i = 0; i < pn_len; ++i)
	// 	printf("%d ", token_queue.tokens[i].id);
}

/**
 * Evaluate the token queue.
 * @return Result of the evaluation.
 * @note Assumes the tokens are in polish notation.
 */
static double mp_evaluate_tokens()
{
	// Operand stack and size
	double* opnd_stack = malloc(sizeof(double));
	size_t opnd_len = 0;
	
	// Pending operand flag
	char pending_operand = 0;
	
	// Loop over every token
	for(size_t i = 0; i < token_queue.len; ++i)
	{
		switch(token_queue.tokens[i].id)
		{
		// If token is an operand...
		case MP_TOKEN_NUM:
			{
				// Convert token to a double
				double operand;
				sscanf(token_queue.tokens[i].str, "%lf", &operand);
			
				// Push it onto the operand stack
				opnd_stack[opnd_len++] = operand;
				opnd_stack = realloc(opnd_stack, sizeof(double) * (opnd_len + 1));
			}
			break;
		
		// If token is a negation...
		case MP_TOKEN_NEG:
			{
				// Make operand negative
				opnd_stack[opnd_len - 1] = -opnd_stack[opnd_len - 1];
			}
			break;
		
		// Otherwise it is an operator...
		default:
			{
				// Pop operands from the stack
				const double operand_1 = opnd_stack[--opnd_len];
				double operand_2 = opnd_stack[--opnd_len];
				opnd_stack = realloc(opnd_stack, sizeof(double) * (opnd_len + 1));
				
				// Compute resulting value and put it in operand 2
				switch(token_queue.tokens[i].id)
				{
				case MP_TOKEN_ADD:
					operand_2 += operand_1;
					break;
					
				case MP_TOKEN_SUB:
					operand_2 -= operand_1;
					break;
					
				case MP_TOKEN_MUL:
					operand_2 *= operand_1;
					break;
					
				case MP_TOKEN_DIV:
					operand_2 /= operand_1;
					break;
				}
				
				// Push result onto the stack
				opnd_stack[opnd_len++] = operand_2;
				opnd_stack = realloc(opnd_stack, sizeof(double) * (opnd_len + 1));
			}
		}
	}
	
	// Result is the final operand
	double res = opnd_stack[0];
	
	// Free stack
	free(opnd_stack);
	
	return res;
}

void mp_parse_all()
{
	// Convert token queue into polish notation
	mp_to_polish_notation();
	
	// Evaluate tokens
	const double eval = mp_evaluate_tokens();
	
	// Print result
	printf("%lf\n", eval);
	
	// Flush the token queue
	mp_flush_parser_tokens();
}