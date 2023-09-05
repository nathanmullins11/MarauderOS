#include "mpx/device.h"
#include <sys_req.h>
#include <help.h>
#include <string.h>

void help(const char *cmd) {
    // build help strings for each command
    char msg_version[] = "Command: version\n\nDescription:\n  Prints the current version of MarauderOS and last build date to the terminal.\n\nUsage:\n  version\n";
    char msg_shutdown[] = "Command: shutdown\n\nDescription:\n  Exits MarauderOS immediately.\n\nUsage:\n  shutdown\n";
    char msg_getdate[] = "Command: getdate\n\nDescription:\n  Prints the current date of MarauderOS to the terminal.\n\nUsage:\n  getdate\n";
    char msg_all[] = "Available Commands\n - version\n - shutdown\n - getdate\n - setdate\n";

    if ( strcmp(cmd, "version") == 0 ) {
        // print help message for 'version' command
        sys_req(WRITE, COM1, msg_version, sizeof(msg_version));
    } else if ( strcmp(cmd, "shutdown") == 0 ) {
        // print help message for 'shutdown' command
        sys_req(WRITE, COM1, msg_shutdown, sizeof(msg_shutdown));
    } else if ( strcmp(cmd, "getdate") == 0 ) {
        // print help message for 'getdate' command
        sys_req(WRITE, COM1, msg_getdate, sizeof(msg_getdate));
    } else {
        // if not parameters -> print all help messages
        sys_req(WRITE, COM1, msg_all, sizeof(msg_all));
    }
}
