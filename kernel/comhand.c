#include <comhand.h>
#include <sys_req.h>

void comhand(void)
{
    for ( ;; ) 
    {
    	char buf[100] = { 0 };
        sys_req(READ, COM1, buf, sizeof(buf));
    }
}
