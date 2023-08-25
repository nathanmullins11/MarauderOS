/**
 * @file version.c
 * @brief function for user to call by typing the command 'version' into the terminal. Function then displays current version as well as most recent compilation date
 */


#include <sys_req.h>
#include <string.h>


void version(void) {
    
	char cur_ver[] = "v1.0 last compiled on 8/25/2023 \n";
	sys_req(WRITE, COM1, cur_ver , sizeof(cur_ver) ) ;


}
