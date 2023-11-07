
#include "libcli/libcli.h"
#include "emcsh_commands.h"


const command shell_commands[] = {
  {
      .name = "hello",
      .callback = cmd_hello,
      .priviledge = PRIVILEGE_UNPRIVILEGED,
      .mode = MODE_EXEC,
      .help = "Hello <password> <client name> <protocol version>",
      .arguments = {
        { 
          .name = "password",
          .flags = CLI_CMD_ARGUMENT,
          .priviledge = PRIVILEGE_UNPRIVILEGED,
          .mode = MODE_EXEC,
          .help = "set password"
        },
        {
          .name = "client_name",
          .flags = CLI_CMD_ARGUMENT,
          .priviledge = PRIVILEGE_UNPRIVILEGED,
          .mode = MODE_EXEC,
          .help = "set client name"
        },
        {
          .name = "protocol_version",
          .flags = CLI_CMD_ARGUMENT,
          .priviledge = PRIVILEGE_UNPRIVILEGED,
          .mode = MODE_EXEC,
          .help = "set protocol version"
        },
        { NULL }     
      },
      .children = { NULL }
    },
    {
      .name = "baaz",
      .callback = cmd_hello,
      .priviledge = PRIVILEGE_UNPRIVILEGED,
      .mode = MODE_EXEC,
      .help = "Get <LinuxCNC command>",
      .arguments = {
        { 
          .name = "command",
          .flags = CLI_CMD_ARGUMENT,
          .priviledge = PRIVILEGE_UNPRIVILEGED,
          .mode = MODE_EXEC,
          .help = NULL
        },
        { NULL }
      },
      .children = {
        &(command) {
          .name = "foo",
          .callback = cmd_hello,
          .priviledge = PRIVILEGE_UNPRIVILEGED,
          .mode = MODE_EXEC,
          .help = "foo",
          .children = { NULL }
        },
        NULL
      }
    },
    { NULL }        // terminate list
  };