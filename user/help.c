#include "mpx/device.h"
#include <sys_req.h>
#include <help.h>
#include <string.h>

void help(const char *cmd) {
    // build help strings for each command
    char msg_version[] = "version :\tdisplays the current version of MarauderOS and last build date\n";
    char msg_shutdown[] = "shutdown : \tquits MarauderOS\n";

    if ( strcmp(cmd, "version") == 0) {
        // print help message for 'version' command
        sys_req(WRITE, COM1, msg_version, sizeof(msg_version));
    } else if ( strcmp(cmd, "shutdown") == 0) {
        // print help message for 'shutdown' command
        sys_req(WRITE, COM1, msg_shutdown, sizeof(msg_shutdown));
    } else {
        // if not parameters -> print all help messages
        sys_req(WRITE, COM1, msg_version, sizeof(msg_version));
        sys_req(WRITE, COM1, msg_shutdown, sizeof(msg_shutdown));
    }
}
