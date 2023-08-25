#include <sys_req.h>
#include <string.h>

void comhand(void)
{
	for ( ; ; ) {
	char buffer[201] = { 0 };
	sys_req(READ, COM1, buffer , sizeof(buffer) ) ;
	}
}
