/** Includes. */
#include "lexer.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "parser.h"

// Structure returned from mp_read_real
typedef struct
{
	// String containing a real number. (Allocated with malloc, must be freed.)
	char* str;
	
	// Number of characters read while reading the number.
	size_t delta;
	
} mp_read_real_data;

/**
 * Function used by mp_lex_string to extract a real number from the input string.
 * @param Input string.
 * @return See mp_read_real_data.
 */
static mp_read_real_data mp_read_real(const char* str)
{
	// String length
	const size_t len = strlen(str);
	
	// List of numbers
	const char nums[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};
	
	// Loop over the string
	size_t num_len;
	for(num_len = 0; num_len < len; ++num_len)
	{
		// If the character isn't a number or a 
		// decimal point, we've reached the end
		// of the number.
		
		if(str[num_len] == '.') continue;
		
		char found_num = 0;
		for(size_t i = 0; i < 10; ++i)
			if(str[num_len] == nums[i])
			{
				found_num = 1;
				break;
			}
		if(found_num == 1) continue;
			
		// Otherwise, we've reached the end of the number
		break;
	}
	
	// Return value data
	mp_read_real_data data;
	
	// If the number of characters read is 0, we
	// didn't read a number
	if(num_len == 0)
	{
		data.str = NULL;
		data.delta = 0;
	}
	else
	{
		// Create a buffer big enough for the number
		data.str = malloc(num_len + 1);
		
		// Update delta
		data.delta = num_len;
		
		// Copy data into the buffer
		memcpy(data.str, str, num_len);
		
		// Add null terminator
		data.str[num_len] = '\0';	
	}
	
	return data;
}

void mp_lex_string(const char* str)
{
	// String length
	const size_t len = strlen(str);
	
	// Return data from reading a real number
	mp_read_real_data real_num;
	
	// Loop over the string
	for(size_t i = 0; i < len; ++i)
	{
		// Read the character
		const char c = str[i];
		
		// Token to add
		token t;
		
		// Ignore whitespace
		if(c == ' ') continue;
		
		// Addition token
		else if(c == '+')
		{
			t.id = MP_TOKEN_ADD;
			t.str = NULL;
		}
		
		// Subtraction token
		else if(c == '-')
		{
			t.id = MP_TOKEN_SUB;
			t.str = NULL;
		}
		
		// Multiplication token
		else if(c == '*')
		{
			t.id = MP_TOKEN_MUL;
			t.str = NULL;
		}
		
		// Division token
		else if(c == '/')
		{
			t.id = MP_TOKEN_DIV;
			t.str = NULL;
		}
		
		// Real number token
		else if((real_num = mp_read_real(str + i)).str != NULL)
		{
			t.id = MP_TOKEN_NUM;
			t.str = real_num.str;
			i += real_num.delta - 1;
		}
		
		// Unkown token
		else 
		{
			printf("UNEXPECTED TOKEN!!!\n");
		}
		
		// Add the token
		mp_add_token_to_parser(t);
	}
}