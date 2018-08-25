/** Includes. */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lexer.h"

int mp_token_precedence[5] =
{
	1,	// Number
	2,	// Add
	2,	// Subtract
	3,	// Multiply
	3	// Divide
};

char mp_token_assoc[5] =
{
	MP_LEFT_ASSOC,	// Number
	MP_LEFT_ASSOC,  // Add
	MP_LEFT_ASSOC,  // Subtract
	MP_LEFT_ASSOC,  // Multiply
	MP_LEFT_ASSOC   // Divide
};

/** Number of tokens to allocate at a time. */
#define MP_TOKEN_CHUNK_SIZE 8

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
	token* pn_tokens = malloc(sizeof(token));
	size_t pn_len = 0;
	
	// Operator token stack and size
	token* op_tokens = malloc(sizeof(token));
	size_t op_len = 0;
	
	// Loop over every token in the original token list
	for(size_t i = 0; i < token_queue.len; ++i)
	{
		// Get current token ID (For convenience)
		const int tok = token_queue.tokens[i].id;
		
		// Number
		if(tok == MP_TOKEN_NUM)
		{
			// Add token to queue
			pn_tokens[pn_len++] = token_queue.tokens[i];			
			pn_tokens = realloc(pn_tokens, sizeof(token) * (pn_len + 1));
		}
		// Must be an operator
		else
		{
			// Pop operator tokens if needed
			if(op_len != 0)
			{
				// Operator token for convenience
				int op_tok = op_tokens[op_len - 1].id;
			
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
						)
					)
				)
				{	
					// Pop operator off the stack 
					pn_tokens[pn_len++] = op_tokens[op_len - 1];
					pn_tokens = realloc(pn_tokens, sizeof(token) * (pn_len + 1));
					
					// Update operator stack length and operator token
					if(--op_len != 0) op_tok = op_tokens[op_len - 1].id;
				}
			}
			
			// Push operator onto the stack
			op_tokens[op_len++] = token_queue.tokens[i];
			op_tokens = realloc(op_tokens, sizeof(token) * (op_len + 1));
		}
	}
	
	// Add remaining operators onto the stack
	if(op_len != 0) for(size_t i = op_len - 1;; --i)
	{
		// Pop operator off the stack 
		pn_tokens[pn_len++] = op_tokens[i];
		pn_tokens = realloc(pn_tokens, sizeof(token) * (pn_len + 1));
	
		if(i == 0) break;
	}
	
	// Free original tokens
	free(token_queue.tokens);
	
	// Update tokens
	token_queue.tokens = pn_tokens;
	
	// Free operator stack
	free(op_tokens);
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
		// If token is an operand...
		if(token_queue.tokens[i].id == MP_TOKEN_NUM)
		{
			// Convert token to a double
			double operand;
			sscanf(token_queue.tokens[i].str, "%lf", &operand);
		
			// Push it onto the operand stack
			opnd_stack[opnd_len++] = operand;
			opnd_stack = realloc(opnd_stack, sizeof(double) * (opnd_len + 1));
		}
		// Otherwise it is an operator...
		else
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