#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"  // MAXARG defines the maximum number of arguments

#define is_blank(chr) (chr == ' ' || chr == '\t') 

int main(int argc, char *argv[]) {
    char buf[2048], charactor;
    char *p = buf;
    char *initial_command[MAXARG];
    int count = 0;
    int blanks = 0;
    int offset = 0;

    if (argc <= 1) {
        exit(1);
    }

    // Copy the initial command arguments
   
	while (count < argc)
	{
		initial_command[count] = argv[count + 1];
		++count;
	}
   

    while (read(0, &charactor, 1) > 0) { // Read input character by character
        if (is_blank(charactor)) { 
            blanks++;
            continue;
        }

        if (blanks) {  // If there were blanks space before, mark the end of a word
            buf[offset++] = 0;
            initial_command[count++] = p; // Store pointer to the argument
            p = buf + offset;
            blanks = 0;
        }

        if (charactor != '\n') {
            buf[offset++] = charactor; // Store character in buffer
        } else { // End of line, execute command
            
			
            initial_command[count++] = p;
			p = buf + offset;

            if (!fork()) { // Create child process
                exit(exec(initial_command[0], initial_command)); // Execute command
            }
			
            wait(0); // Wait for child process to finish
            count = argc - 1; // Reset argument count
			//End after this
        }
    }
    
    exit(0);
}
