#include "mpx/device.h"
#include <sys_req.h>
#include <help.h>
#include <string.h>

void help(const char *cmd) {
    // build help strings for each command
    char msg_version[] = "Command: version\n\nDescription:\n  Prints the current version of MarauderOS and last build date to the terminal.\n\nUsage:\n  version\n";
    char msg_shutdown[] = "Command: shutdown\n\nDescription:\n  Exits MarauderOS, ask for confirmation before initiating shutdown.\n\nUsage:\n  shutdown\n\nAdditional Notes:\n  Running `shutdown!` will immediately exit the OS without confirmation\n";
    char msg_getdate[] = "Command: getdate\n\nDescription:\n  Prints the current date of MarauderOS to the terminal.\n\nUsage:\n  getdate\n";
    char msg_setdate[] = "Command: setdate\n\nDescription:\n  Sets the date of the MarauderOS.\n\nUsage:\n  setdate [options] [argument]\n\nOptions:\n  -d,\tThe new date in mm-dd-yy format\n\nExample:\n  setdate -d 06-29-07\n";
    char msg_gettime[] = "Command: gettime\n\nDescription:\n  Prints the current time of MarauderOS to the terminal, format in 24-hour GMT.\n\nUsage:\n  gettime\n";
    char msg_settime[] = "Command: settime\n\nDescription:\n  Sets the time of the MarauderOS, in 24-hour GMT format.\n\nUsage:\n  settime [options] [argument]\n\nOptions\n  -t,\tThe new time in hh:mm:ss format\n\nExample:\n  settime -t 09:41:00\n";
    char msg_pcb[] = "Command: pcb\n\nDescription:\n  Handles all Process Control Block (PCB) related commands and actions.\n\nUsage:\n  pcb [options] [process name] [suboptions] [suboption arguments]\n\n";
    char msg_pcb2[] = "Options:\n  -d,\tDeletes a specified PCB, cannot be a system process\n  -b,\tBlocks a specified PCB\n  -u,\tUnblocks a specified PCB\n  -s,\tSuspends a specified PCB\n  -r,\tResumes a specified PCB\n  -p,\tChanges an existing PCB's priority, requires --pri suboption\n  -l,\tLists PCB information, takes the following as arguments: ready, blocked, [process_name], or no argument\n  \tto show all processes\n\n";
    char msg_pcb3[] = "Suboptions:\n  --pri,\tProcess's priority, an integer between 0 and 9, inclusive\n\n";
    char msg_pcb4[] = "Examples:\n  - Create a PCB:\t\tpcb -c NAME --class user --pri 4\n  - Delete a PCB:\t\tpcb -d NAME\n  - Change priority:\t\tpcb -p NAME --pri 1\n  - List all blocked processes:\tpcb -l blocked\n\nAdditional Notes:\n  Refer to the User Manuel for a more in-depth explanation\n  (https://github.com/WVU-CS450/Morgantown-Marauders-/blob/main/doc/UserManual.pdf)\n  INFO: Create PCB no longer supported as of v3.0\n";
    char msg_loadr3[] = "Command: load_r3\n\nDescription:\n  Loads five faux processes into the ready queue. These processes print to the terminal.\n\nUsage:\n  load_r3\n";
    char msg_yield[] = "Command: yield\n\nDescription:\n  Command handler yields to the CPU, runs any processes in the ready queue.\n\nUsage:\n  yield\n";
    char msg_all[] = "Available Commands | type `help [command]` for more detailed help\n - version\n - shutdown\n - getdate\n - setdate\n - gettime\n - settime\n - pcb\n - load_r3\n - yield\n";

    if ( strcmp(cmd, "version") == 0 ) {
        // print help message for 'version' command
        sys_req(WRITE, COM1, msg_version, sizeof(msg_version));
    } else if ( strcmp(cmd, "shutdown") == 0 ) {
        // print help message for 'shutdown' command
        sys_req(WRITE, COM1, msg_shutdown, sizeof(msg_shutdown));
    } else if ( strcmp(cmd, "getdate") == 0 ) {
        // print help message for 'getdate' command
        sys_req(WRITE, COM1, msg_getdate, sizeof(msg_getdate));
    } else if ( strcmp(cmd, "setdate") == 0 ) {
        // print help message for 'setdate' command
        sys_req(WRITE, COM1, msg_setdate, sizeof(msg_setdate));
    } else if ( strcmp(cmd, "gettime") == 0 ) {
        // print help message for 'gettime' command
        sys_req(WRITE, COM1, msg_gettime, sizeof(msg_gettime));
    } else if ( strcmp(cmd, "settime") == 0 ) {
        // print help message for 'settime' command
        sys_req(WRITE, COM1, msg_settime, sizeof(msg_settime));
    } else if ( strcmp(cmd, "pcb") == 0 ) {
        // print help message for 'pcb' command
        sys_req(WRITE, COM1, msg_pcb, sizeof(msg_pcb));
        sys_req(WRITE, COM1, msg_pcb2, sizeof(msg_pcb2));
        sys_req(WRITE, COM1, msg_pcb3, sizeof(msg_pcb3));
        sys_req(WRITE, COM1, msg_pcb4, sizeof(msg_pcb4));
    } else if ( strcmp(cmd, "load_r3") == 0 ) {
        // print help message for load_r3 command
        sys_req(WRITE, COM1, msg_loadr3, strlen(msg_loadr3));
    } else if ( strcmp(cmd, "yield") == 0 ) {
        // print help message for yield command
        sys_req(WRITE, COM1, msg_yield, strlen(msg_yield));
    } else {
        // if not parameters -> print all help messages
        sys_req(WRITE, COM1, msg_all, sizeof(msg_all));
    }
}
