/** Includes. */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// Buffer step size when allocating memory for user input.
#define MP_INPUT_BUFFER_STEP 16

char* mp_get_user_input(void)
{
	// Input buffer
	char* buff = malloc(MP_INPUT_BUFFER_STEP);
	
	// Size of the buffer
	size_t buff_len = MP_INPUT_BUFFER_STEP;
	
	// Length of input
	size_t len = 0;
	
	// Read characters in a loop
	char c;
	while(c = (char)getchar())
	{
		// End at EOF or new line
		if(c == '\n' || c == EOF)
			break;
		
		// Increase string length
		++len;
		
		// Check if we need to resize the buffer
		if(len > buff_len)
		{
			// Create a new buffer
			char* new_buff = malloc(buff_len + MP_INPUT_BUFFER_STEP);
			
			// Copy contents of original buffer into the new buffer
			memcpy(new_buff, buff, buff_len);
			
			// Update buffer size
			buff_len += MP_INPUT_BUFFER_STEP;
			
			// Free old buffer
			free(buff);
			
			// Set new buffer
			buff = new_buff;
		}
		
		// Put character into the buffer
		buff[len - 1] = c;
	}
	
	// Create a new buffer of correct size
	char* final_buff = malloc(len + 1);
	
	// Copy original buffer into the final buffer
	memcpy(final_buff, buff, len);
	
	// Add null terminator
	final_buff[len] = '\0';
	
	// Free original buffer
	free(buff);
	
	return final_buff;
}