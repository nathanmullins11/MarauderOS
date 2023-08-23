#include <sys_req.h>
#include <string.h>

void comhand(void)
{
	for ( ; ; ) {
	char buffer[201] = { 0 };
	int bytes_read = sys_req(READ, COM1, buffer , sizeof(buffer) ) ;
    sys_req(WRITE, COM1, buffer , bytes_read ) ;
	}
}
