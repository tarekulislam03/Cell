#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "ui.h"

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

    char initial_cwd[1024];

    // Save starting directory for prompt formatting
    if (getcwd(initial_cwd, sizeof(initial_cwd)) == NULL) {
        perror("Failed to get starting directory");
        exit(EXIT_FAILURE);
    }

    int init_len = strlen(initial_cwd);

    // Render boot banner
    print_boot_banner();

    while (1) {
        char line[512];
        char cwd[1024];
        char prompt[1200];

        Theme t = get_current_theme();

        // Format CWD for prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            if (strncmp(cwd, initial_cwd, init_len) == 0) {
                char *suffix = cwd + init_len;
                if (strlen(suffix) == 0) {
                    snprintf(prompt, sizeof(prompt),
                             "\001%s\002 ~ \001%s\002", t.primary, t.text);
                } else {
                    snprintf(prompt, sizeof(prompt),
                             "\001%s\002 %s ~ \001%s\002", t.primary, suffix, t.text);
                }
            } else {
                snprintf(prompt, sizeof(prompt),
                         "\001%s\002 %s ~ \001%s\002", t.primary, cwd, t.text);
            }
        } else {
            snprintf(prompt, sizeof(prompt),
                     "\001%s\002 ~ \001%s\002", t.primary, t.text);
        }

        // Read command line
        char *raw_line = readline(prompt);

        if (raw_line == NULL) {
            printf("\n%s[SESSION TERMINATED]%s\n", t.highlight, t.reset);
            break; // Handle Ctrl+D (EOF)
        }

        if (strlen(raw_line) > 0) {
            add_history(raw_line);
        }

        strncpy(line, raw_line, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
        free(raw_line);

        // Tokenize input string with quote support ("..." and '...')
        char *my_args[32];
        tokenize_input(line, my_args, 32);

        if (my_args[0] == NULL) continue;

        // Built-in Shell & Hacker Commands
        if (strcmp(my_args[0], "exit") == 0 || strcmp(my_args[0], "quit") == 0) {
            printf("%s[+] Exiting CELL console. Goodbye, agent.%s\n", t.highlight, t.reset);
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

        if (strcmp(my_args[0], "scan") == 0 || strcmp(my_args[0], "hack") == 0) {
            run_hacker_scanner(my_args[1]);
            continue;
        }

        if (strcmp(my_args[0], "theme") == 0) {
            if (my_args[1] == NULL) {
                print_available_themes();
            } else {
                if (set_theme(my_args[1])) {
                    Theme new_t = get_current_theme();
                    printf("  %stheme switched to '%s'%s\n\n", new_t.primary, new_t.name, new_t.reset);
                } else {
                    printf("  %sunknown theme. options: matrix, cyberpunk, amber, synthwave, blood, monokai%s\n\n", t.highlight, t.reset);
                }
            }
            continue;
        }

        if (strcmp(my_args[0], "cd") == 0) {
            if (my_args[1] == NULL) {
                char *home = getenv("HOME");
                if (home) {
                    if (chdir(home) != 0) perror("cd failed");
                } else {
                    fprintf(stderr, "cd: missing argument\n");
                }
            } else if (chdir(my_args[1]) != 0) {
                perror("cd failed");
            }
            continue;
        }

        // Fork & exec external Linux process
        pid_t pid = fork();
        int status;

        if (pid == 0) {
            execvp(my_args[0], my_args);
            printf("%s[!] Command not found or exec failed: %s%s\n", t.highlight, my_args[0], t.reset);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            wait(&status);
        } else {
            perror("fork failed");
        }
    }

    return EXIT_SUCCESS;
}