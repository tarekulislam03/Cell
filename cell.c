#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "ui.h"

// Tokenize input string into arguments
static void tokenize_input(char *line, char **args, int max_args) {
    int i = 0;
    char *p = line;
    while (*p && i < max_args - 1) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') break;

        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            args[i++] = p;
            while (*p && *p != quote) p++;
            if (*p) {
                *p = '\0';
                p++;
            }
        } else {
            args[i++] = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
        }
    }
    args[i] = NULL;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Render boot banner
    print_boot_banner();

    while (1) {
        char line[512];
        char cwd[1024];
        char prompt[1200];

        // Format cwd for prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            char display_path[1024];
            if (strncmp(cwd, "/home/", 6) == 0) {
                snprintf(display_path, sizeof(display_path), "%s", cwd + 6);
            } else {
                snprintf(display_path, sizeof(display_path), "%s", cwd);
            }

            snprintf(prompt, sizeof(prompt),
                     "\001\033[1;32m\002%s ~\001\033[0m\002\n\001\033[1;33m\002$ \001\033[0m\002",
                     display_path);
        } else {
            snprintf(prompt, sizeof(prompt),
                     "\001\033[1;32m\002tarekul ~\001\033[0m\002\n\001\033[1;33m\002$ \001\033[0m\002");
        }

        // Read command
        fflush(stdout);
        char *raw_line = readline(prompt);

        if (raw_line == NULL) {
            printf("\n%s[SESSION TERMINATED]%s\n", COLOR_HIGHLIGHT, COLOR_RESET);
            break;
        }

        if (strlen(raw_line) > 0) {
            add_history(raw_line);
        }

        strncpy(line, raw_line, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
        free(raw_line);

        char *my_args[32];
        tokenize_input(line, my_args, 32);

        if (my_args[0] == NULL) continue;

        // Built-in Commands
        if (strcmp(my_args[0], "exit") == 0 || strcmp(my_args[0], "quit") == 0) {
            printf("%sFarewell.%s\n", COLOR_HIGHLIGHT, COLOR_RESET);
            fflush(stdout);
            break;
        }

        if (strcmp(my_args[0], "clear") == 0 || strcmp(my_args[0], "cls") == 0) {
            clear_screen();
            continue;
        }

        if (strcmp(my_args[0], "help") == 0) {
            print_help();
            continue;
        }

        if (strcmp(my_args[0], "sysinfo") == 0 || strcmp(my_args[0], "neofetch") == 0) {
            print_sysinfo();
            continue;
        }

        if (strcmp(my_args[0], "matrix") == 0) {
            int duration = 4;
            if (my_args[1] != NULL) {
                duration = atoi(my_args[1]);
                if (duration <= 0) duration = 4;
            }
            run_matrix_animation(duration);
            continue;
        }

        if (strcmp(my_args[0], "theme") == 0) {
            if (my_args[1] != NULL) {
                printf("\033]777;theme;%s\007", my_args[1]);
                fflush(stdout);
                printf("\n  %sTheme switched to '%s'%s\n\n", COLOR_PRIMARY, my_args[1], COLOR_RESET);
            } else {
                printf("\n  %sAvailable Themes:%s\n", COLOR_HIGHLIGHT, COLOR_RESET);
                printf("    - %ssunset%s    (Warm pixel cabin sunset - default)\n", COLOR_PRIMARY, COLOR_RESET);
                printf("    - %scyberpunk%s (Neon pink & cyan rain street)\n", COLOR_PRIMARY, COLOR_RESET);
                printf("    - %sforest%s    (Emerald moonlit misty pine forest)\n", COLOR_PRIMARY, COLOR_RESET);
                printf("    - %ssynthwave%s (Outrun grid & magenta synth sun)\n", COLOR_PRIMARY, COLOR_RESET);
                printf("    - %smatrix%s    (Hacker green digital code rain)\n", COLOR_PRIMARY, COLOR_RESET);
                printf("\n  Usage: %stheme <name>%s\n\n", COLOR_HIGHLIGHT, COLOR_RESET);
            }
            continue;
        }

        if (strcmp(my_args[0], "scan") == 0 || strcmp(my_args[0], "hack") == 0) {
            run_hacker_scanner(my_args[1]);
            continue;
        }

        if (strcmp(my_args[0], "set") == 0) {
            if (my_args[1] != NULL && my_args[2] != NULL) {
                printf("\033]777;set;%s;%s\007", my_args[1], my_args[2]);
                printf("  %s%s updated to '%s'%s\n\n", COLOR_PRIMARY, my_args[1], my_args[2], COLOR_RESET);
            } else {
                printf("  %susage: set <opacity|blur|font|cursor> <value>%s\n\n", COLOR_HIGHLIGHT, COLOR_RESET);
            }
            fflush(stdout);
            continue;
        }

        if (strcmp(my_args[0], "cd") == 0) {
            if (my_args[1] == NULL) {
                char *home = getenv("HOME");
                if (home) {
                    if (chdir(home) != 0)
                        perror("cd failed");
                } else {
                    fprintf(stderr, "HOME variable not set.\n");
                }
            } else if (chdir(my_args[1]) != 0) {
                perror("cd failed");
            }
            continue;
        }

        // Fork & execute external commands
        pid_t pid = fork();
        int status;

        if (pid == 0) {
            execvp(my_args[0], my_args);
            printf("%scommand not found: %s%s\n", COLOR_HIGHLIGHT, my_args[0], COLOR_RESET);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            wait(&status);
        } else {
            perror("fork failed");
        }
    }

    return EXIT_SUCCESS;
}