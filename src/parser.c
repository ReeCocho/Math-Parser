/** Includes. */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "lexer.h"

/** Number of tokens to allocate at a time. */
#define MP_TOKEN_CHUNK_SIZE 8

// Variable data type
typedef struct
{
	/** String containing variable name. */
	char* str;
	
	/** Variable value. */
	double val;
	
} mp_var;

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

int mp_token_precedence[8] =
{
	1,	// Number
	1,	// Variable
	2,	// Add
	2,	// Subtract
	3,	// Multiply
	3,	// Divide
	4,	// Negation
	4,	// Exponentiation
};

char mp_token_assoc[8] =
{
	MP_LEFT_ASSOC,	// Number
	MP_LEFT_ASSOC, 	// Variable
	MP_LEFT_ASSOC,  // Add
	MP_LEFT_ASSOC,  // Subtract
	MP_LEFT_ASSOC,  // Multiply
	MP_LEFT_ASSOC,  // Divide
	MP_RIGHT_ASSOC, // Negation
	MP_RIGHT_ASSOC,	// Exponentiation
};

/** Token queue. */
static struct
{
	/** Pointer to array of token objects. */
	token* tokens;
	
	/** Number of tokens. */
	size_t len;
	
	/** Number of tokens allocated. */
	size_t allocated;
	
} mp_token_queue;

/** Variable list */
static struct
{
	/** Pointer to array of variables. */
	mp_var* vars;
	
	/** Number of variables. */
	size_t len;
	
} mp_vars;



void mp_init_parser()
{
	// Init token queue
	mp_token_queue.tokens = malloc(sizeof(token) * MP_TOKEN_CHUNK_SIZE);
	mp_token_queue.len = 0;
	mp_token_queue.allocated = MP_TOKEN_CHUNK_SIZE;
	
	// Init variable list
	mp_vars.vars = malloc(sizeof(mp_var));
	mp_vars.len = 0;
}

void mp_add_token_to_parser(token t)
{
	// Add token to queue
	mp_token_queue.tokens[mp_token_queue.len++] = t;
	
	// Resize token queue if needed
	if(mp_token_queue.len == mp_token_queue.allocated)
	{
		// Update token allocation counter
		mp_token_queue.allocated += MP_TOKEN_CHUNK_SIZE;
		
		mp_token_queue.tokens = realloc(
			mp_token_queue.tokens, 
			sizeof(token) * mp_token_queue.allocated
		);
	}
}

void mp_flush_parser_tokens()
{
	// Loop over every token
	for(size_t i = 0; i < mp_token_queue.len; ++i)
		// If the token is a number or variable name...
		if(	mp_token_queue.tokens[i].id == MP_TOKEN_NUM ||
			mp_token_queue.tokens[i].id == MP_TOKEN_VAR)
			// Free the string
			free(mp_token_queue.tokens[i].str);

	// Resize token queue
	mp_token_queue.tokens = realloc(
		mp_token_queue.tokens, 
		sizeof(token) * MP_TOKEN_CHUNK_SIZE
	);
	mp_token_queue.len = 0;
	mp_token_queue.allocated = MP_TOKEN_CHUNK_SIZE;
}

void mp_flush_variables()
{
	// Loop over every variable
	for(size_t i = 0; i < mp_vars.len; ++i)
		// Free the variable name
		free(mp_vars.vars[i].str);
	
	// Resize the variable list
	mp_vars.vars = realloc(mp_vars.vars, sizeof(mp_var));
	mp_vars.len = 0;
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
	for(size_t i = 0; i < mp_token_queue.len; ++i)
	{
		// Get current token ID (For convenience)
		const int tok = mp_token_queue.tokens[i].id;
		
		// Number
		if(tok == MP_TOKEN_NUM)
		{
			// Make the value negative if needed
			char* str = mp_token_queue.tokens[i].str;
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
		
		// Variable name
		else if(tok == MP_TOKEN_VAR)
		{
			// Add token to queue
			pn_tokens[pn_len].t = mp_token_queue.tokens[i];
			pn_tokens[pn_len++].flag = next_is_neg;
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
			op_tokens[op_len++].t = mp_token_queue.tokens[i];			
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
			op_tokens[op_len++].t = mp_token_queue.tokens[i];
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
	free(mp_token_queue.tokens);
	
	// Convert pn_tokens to regular tokens
	mp_token_queue.tokens = malloc(sizeof(token) * pn_len);
	mp_token_queue.len = pn_len;
	for(size_t i = 0; i < pn_len; ++i)
		mp_token_queue.tokens[i] = pn_tokens[i].t;
	
	// Free stacks
	free(pn_tokens);
	free(op_tokens);
	
	// // Print loop for debugging
	// for(size_t i = 0; i < pn_len; ++i)
	// 	printf("%d ", mp_token_queue.tokens[i].id);
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
	for(size_t i = 0; i < mp_token_queue.len; ++i)
	{
		switch(mp_token_queue.tokens[i].id)
		{
		// If token is a number...
		case MP_TOKEN_NUM:
			{
				// Convert token to a double
				double operand;
				sscanf(mp_token_queue.tokens[i].str, "%lf", &operand);
			
				// Push it onto the operand stack
				opnd_stack[opnd_len++] = operand;
				opnd_stack = realloc(opnd_stack, sizeof(double) * (opnd_len + 1));
			}
			break;
			
		// If the token is a variable
		case MP_TOKEN_VAR:
			{
				// Loop for the variable in the list
				char found = 0;
				for(size_t i = 0; i < mp_vars.len; ++i)
					if(strcmp(
						mp_vars.vars[i].str, 
						mp_token_queue.tokens[i].str
						) == 0
					)
					{
						// Push it onto the operand stack
						opnd_stack[opnd_len++] = mp_vars.vars[i].val;
						opnd_stack = realloc(
							opnd_stack, 
							sizeof(double) * (opnd_len + 1)
						);
						found = 1;
						break;
					}
					
				// If we didn't find it, throw an error
				if(found == 0) 
					printf(
						"Unable to locate variable \"%s\"", 
						mp_token_queue.tokens[i].str
					);
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
				switch(mp_token_queue.tokens[i].id)
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
					
				case MP_TOKEN_EXP:
					operand_2 = pow(operand_2, operand_1);
					break;
				}
				
				// Push result onto the stack
				opnd_stack[opnd_len++] = operand_2;
				opnd_stack = realloc(opnd_stack, sizeof(double) * (opnd_len + 1));
			}
		}
	}
	
	// Result is the final operand
	double res = opnd_len == 0 ? 0.0 : opnd_stack[0];
	
	// Free stack
	free(opnd_stack);
	
	return res;
}

void mp_parse_all()
{
	// Detect if we are assigning a variable a value
	if(
		// Must have at least two tokens
		mp_token_queue.len >= 2 &&
		// First must be a variable name
		mp_token_queue.tokens[0].id == MP_TOKEN_VAR &&
		// Second must be an equals sign
		mp_token_queue.tokens[1].id == MP_TOKEN_EQL
	)
	{
		// Grab the variable name before we delete it
		token var = mp_token_queue.tokens[0];
	
		// Shift the token queue two elements down
		// (Removing the variable name and equals sign)
		memmove(
			mp_token_queue.tokens, 
			&mp_token_queue.tokens[2], 
			sizeof(token) * (mp_token_queue.len -= 2)
		);
		
		// Convert token queue into polish notation
		mp_to_polish_notation();
		
		// Evaluate tokens
		const double eval = mp_evaluate_tokens();
		
		// Look for the token
		char found = 0;
		for(size_t i = 0; i < mp_vars.len; ++i)
			if(strcmp(mp_vars.vars[i].str, var.str) == 0)
			{
				// Update token value
				mp_vars.vars[i].val = eval;
				found = 1;
				break;
			}
			
		// If we didn't find the variable, add it to the list
		if(found == 0)
		{
			mp_vars.vars[mp_vars.len].str = var.str;
			mp_vars.vars[mp_vars.len++].val = eval;
			mp_vars.vars = realloc(mp_vars.vars, sizeof(mp_var) * (mp_vars.len + 1));
		}
	}
	// Must be evaluating an expression...
	else
	{
		// Convert token queue into polish notation
		mp_to_polish_notation();
		
		// Evaluate tokens
		const double eval = mp_evaluate_tokens();
		
		// Print result
		printf("%lf\n", eval);
	}
	
	// Flush the token queue
	mp_flush_parser_tokens();
}