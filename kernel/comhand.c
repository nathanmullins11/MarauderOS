#include <comhand.h>
#include <sys_req.h>
#include <itoa.h>
#include <string.h>
#include <version.h>
#include <time.h>

void comhand(void)
{
    for ( ;; ) 
    {
    	char buf[100] = {0};
        int size_buffer = sys_req(READ, COM1, buf, sizeof(buf));

		// check if the buffer is ended with a null terminator before evaluating content
		if (buf[size_buffer] == '\0') {
			// check if to run 'version' command
			if ( strcmp("version", buf) == 0 ) {
				version();
			} else if ( strcmp("shutdown", buf) == 0 ) {
				return;
			} else if ( strcmp("get date", buf) == 0 ) {
				get_date();
			}else {
				char error_msg[21] = "ERR: Invalid Command\n";
				sys_req(WRITE, COM1, error_msg, strlen(error_msg));
			}

		}
    }
}
