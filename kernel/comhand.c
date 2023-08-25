#include <comhand.h>
#include <sys_req.h>
#include <itoa.h>
#include <string.h>

void comhand(void)
{
    for ( ;; ) 
    {
    	char buf[100] = { 0 };
        int size_buffer = sys_req(READ, COM1, buf, sizeof(buf));

		if (buf[size_buffer] == '\0') {
			// char test[100] = {"test"};
			// sys_req(WRITE, COM1, test, sizeof(test));

			if ( strcmp({"version"}, buf) == 0 ) {
				
			}
		}
    }
}
