/** Includes. */
#include "stdlib.h"
#include "lexer.h"

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

/** 
 * Expression return value. 
 */
typedef struct
{
	/** Expression value. */
	double value;
	
	/** Number of tokens read. */
	size_t stride;
	
} mp_expression_result;



void mp_init_parser()
{
	token_queue.tokens = malloc(sizeof(token) * MP_TOKEN_CHUNK_SIZE);
	token_queue.len = 0;
	token_queue.allocated = MP_TOKEN_CHUNK_SIZE;
}

void mp_add_token_to_parser(token t)
{
	// Add token to queue
	token_queue.tokens[++token_queue.len] = t;
	
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
	// Free old token queue
	free(token_queue.tokens);
	
	// Reinitialize the parser
	mp_init_parser();
}



// Forward declaration
static mp_expression_result mp_parse_primary_expression(const size_t offset);

/**
 * Parse an additive expression.
 * @param Offset in the token queue.
 * @return Result of the expression.
 */
static mp_expression_result mp_parse_additive_expression(const size_t offset)
{
	size_t stride = 0;
	
	// Read left hand side expression
	const mp_expression_result lhs = mp_parse_primary_expression(offset + stride);
	stride += lhs.stride;
	
	// Read operator
	const token op = token_queue.tokens[offset + stride];
	++stride;
	
	// Read right hand side expression
	const mp_expression_result rhs = mp_parse_primary_expression(offset + stride);
	stride += rhs.stride;
	
	// Compute result
	mp_expression_result res;
	res.value = op.id == MP_TOKEN_ADD ? 
				(lhs.value + rhs.value) : 
				(lhs.value - rhs.value);
	res.stride = stride - offset;
	
	return res;
}

/**
 * Parse a multiplicative expression.
 * @param Offset in the token queue.
 * @return Result of the expression.
 */
static mp_expression_result mp_parse_multiplicative_expression(const size_t offset)
{
	size_t stride = 0;
	
	// Read left hand side expression
	const mp_expression_result lhs = mp_parse_primary_expression(offset + stride);
	stride += lhs.stride;
	
	// Read operator
	const token op = token_queue.tokens[offset + stride];
	++stride;
	
	// Read right hand side expression
	const mp_expression_result rhs = mp_parse_primary_expression(offset + stride);
	stride += rhs.stride;
	
	// Compute result
	mp_expression_result res;
	res.value = op.id == MP_TOKEN_MUL ? 
				(lhs.value * rhs.value) : 
				(lhs.value / rhs.value);
	res.stride = stride;
	
	return res;
}

/**
 * Parse a primary expression.
 * @param Offset in the token queue.
 * @return Result of the expression.
 */
static mp_expression_result mp_parse_primary_expression(const size_t offset)
{
	size_t stride = 0;
	
	//
}

void mp_parse_all()
{
	// Loop over every token
	for(size_t i = 0; i < token_queue.len; ++i)
	{
		
	}
	
	// Flush the token queue
	mp_flush_parser_tokens();
}