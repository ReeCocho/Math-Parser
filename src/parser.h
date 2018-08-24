#ifndef MP_PARSER_H
#define MP_PARSER_H

/**
 * Functions to take tokens and process them as expressions.
 */
 
/** Includes. */
#include "lexer.h"

/**
 * Initialize the parser.
 */
extern void mp_init_parser();

/**
 * Add a token to the parser token queue.
 * @param Token to add.
 */
extern void mp_add_token_to_parser(token t);

/**
 * Flush the parsers token queue.
 */
extern void mp_flush_parser_tokens();

/**
 * Parse and execute the expressions described in the token queue.
 */
extern void mp_parse_all();
#endif