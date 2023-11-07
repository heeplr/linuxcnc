#ifndef EMCSH_COMMANDS
#define EMCSH_COMMANDS

#ifdef __cplusplus
extern "C" {
#endif


// maximum amount of arguments a cli command can have
#define MAX_CMD_ARGS 16
// maximum amount of cli subcommands a command can have
#define MAX_SUB_CMDS 8

int cmd_hello(struct cli_def *cli, const char *command, char *argv[], int argc);

/* one linuxcnc shell command */
typedef struct command {
  /* printable name of command */
  const char *name;
  /* the actual command function */
  int (*callback)(struct cli_def *, const char *, char **, int);
  /* PRIVILEDGE_UNPRIVILEDGED or PRIVILEDGE_PRIVILEDGED */
  const int priviledge;
  /* cli_command mode */
  const int mode;
  /* helptext to display with builtin "help" command */
  const char *help;
  /* list of arguments for this command */
  struct commandArg {
    /* printable name of argument */
    const char *name;
    /* cli_optarg flags */
    const int flags;
    /* PRIVILEDGE_UNPRIVILEDGED or PRIVILEDGE_PRIVILEDGED */
    const int priviledge;
    /* cli_optarg mode */
    const int mode;
    /* helptext for argument */
    const char *help;
    /* custom completor */
    int (*get_completions)(struct cli_def *cli, const char *, const char *, struct cli_comphelp *);
    /* custom validator */
    int (*validator)(struct cli_def *cli, const char *, const char *);
    int (*transient_mode)(struct cli_def *, const char *, const char *);
  } arguments[MAX_CMD_ARGS];
  /* subcommands of this command */
  struct command *children[MAX_SUB_CMDS];
} command;

extern const command shell_commands[];


#ifdef __cplusplus
}
#endif

#endif
