#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <time.h>
#include "ui.h"

static Theme themes[] = {
    // name,       primary (prompt),    secondary,           highlight,           text,                reset
    {"matrix",    "\033[1;32m",       "\033[0;32m",        "\033[1;37m",        "\033[1;33m",        "\033[0m"}, // Green Prompt, Yellow Text
    {"cyberpunk", "\033[1;36m",       "\033[1;35m",        "\033[1;33m",        "\033[1;35m",        "\033[0m"}, // Cyan Prompt, Magenta Text
    {"amber",     "\033[1;33m",       "\033[0;33m",        "\033[1;37m",        "\033[0;33m",        "\033[0m"}, // Gold Prompt, Amber Text
    {"synthwave", "\033[1;35m",       "\033[1;34m",        "\033[1;36m",        "\033[1;36m",        "\033[0m"}, // Purple Prompt, Cyan Text
    {"blood",     "\033[1;31m",       "\033[0;31m",        "\033[1;33m",        "\033[1;33m",        "\033[0m"}, // Red Prompt, Gold Text
    {"monokai",   "\033[1;32m",       "\033[1;34m",        "\033[1;33m",        "\033[1;37m",        "\033[0m"}  // Green Prompt, White Text
};

static int current_theme_idx = 0;
static const int num_themes = sizeof(themes) / sizeof(themes[0]);

Theme get_current_theme(void) {
    return themes[current_theme_idx];
}

int set_theme(const char *name) {
    if (!name) return 0;
    for (int i = 0; i < num_themes; i++) {
        if (strcmp(name, themes[i].name) == 0) {
            current_theme_idx = i;
            return 1;
        }
    }
    return 0;
}

void print_available_themes(void) {
    Theme t = get_current_theme();
    printf("\n");
    printf("  %savailable themes%s : matrix, cyberpunk, amber, synthwave, blood, monokai\n", t.primary, t.reset);
    printf("  %scurrent theme%s   : %s\n", t.primary, t.reset, t.name);
    printf("  %show to change%s   : theme <name>\n\n", t.primary, t.reset);
}

int get_terminal_width(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col > 40 ? w.ws_col : 80;
    }
    return 80;
}

void clear_screen(void) {
    printf("\033[2J\033[H\033[3J");
    fflush(stdout);
}





void run_matrix_animation(int seconds) {
    clear_screen();
    printf("\033[?25l"); // Hide cursor

    int width = get_terminal_width();
    int height = 24;
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        if (w.ws_row > 10) height = w.ws_row;
    }

    int *drops = malloc(width * sizeof(int));
    if (!drops) return;

    for (int i = 0; i < width; i++) {
        drops[i] = rand() % height;
    }

    const char *charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ@#$%&*<>[]{}/*-+~CELL";
    int charset_len = strlen(charset);
    Theme t = get_current_theme();

    time_t start_time = time(NULL);
    while (time(NULL) - start_time < seconds) {
        printf("\033[H");
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (drops[x] == y) {
                    printf("%s%c", t.highlight, charset[rand() % charset_len]);
                } else if ((drops[x] - 1 + height) % height == y) {
                    printf("%s%c", t.primary, charset[rand() % charset_len]);
                } else if ((drops[x] - 2 + height) % height == y) {
                    printf("%s%c", t.secondary, charset[rand() % charset_len]);
                } else {
                    printf(" ");
                }
            }
            printf("\n");
        }

        for (int i = 0; i < width; i++) {
            if (rand() % 10 > 2) {
                drops[i] = (drops[i] + 1) % height;
            }
        }
        usleep(50000); // 50ms frame rate
    }

    free(drops);
    printf("\033[?25h"); // Show cursor
    clear_screen();
    print_menu_commands();
}

void run_hacker_scanner(const char *target) {
    Theme t = get_current_theme();
    const char *tgt = (target && strlen(target) > 0) ? target : "127.0.0.1 (LOCAL NODE)";

    clear_screen();
    printf("\033[?25l"); // Hide cursor

    int ports[] = {21, 22, 80, 443, 3000, 3306, 5432, 8080};
    const char *services[] = {"FTP", "SSH", "HTTP", "HTTPS", "DEV-NODE", "MYSQL", "POSTGRES", "HTTP-ALT"};
    int num_ports = sizeof(ports) / sizeof(ports[0]);

    // Stage 1: Full-screen port scanning & cipher stream animation
    for (int p = 0; p < num_ports; p++) {
        for (int frame = 0; frame <= 20; frame++) {
            printf("\033[H"); // Cursor to top-left

            // Header HUD
            printf("%s┌── SYSTEM INFILTRATION SCANNER // TARGET: %s%s%s ──┐%s\n",
                   t.primary, t.highlight, tgt, t.primary, t.reset);
            printf("%s│%s MODE: FULL EXPLOIT MATRIX | THREADS: 32 | PROTOCOL: TCP/IP %s│%s\n",
                   t.primary, t.text, t.primary, t.reset);
            printf("%s├─── PORT VULNERABILITY SCANNER ───────────────────────────────────────────────┤%s\n",
                   t.primary, t.reset);

            // Render ports status
            for (int i = 0; i < num_ports; i++) {
                if (i < p) {
                    printf("%s│ %s%-8s (PORT %4d) : %s[████████████████████] %s[ OPEN / EXPLOITED ] %s│%s\n",
                           t.primary, t.text, services[i], ports[i], t.primary, t.highlight, t.primary, t.reset);
                } else if (i == p) {
                    char bar[21];
                    for (int b = 0; b < 20; b++) {
                        bar[b] = (b < frame) ? '#' : '.';
                    }
                    bar[20] = '\0';
                    printf("%s│ %s%-8s (PORT %4d) : %s[%-20s] %s[ BYPASSING... ]   %s│%s\n",
                           t.primary, t.text, services[i], ports[i], t.highlight, bar, t.secondary, t.primary, t.reset);
                } else {
                    printf("%s│ %s%-8s (PORT %4d) : %s[....................] %s[ WAITING... ]      %s│%s\n",
                           t.primary, t.text, services[i], ports[i], t.secondary, t.secondary, t.primary, t.reset);
                }
            }

            printf("%s├─── REALTIME MEMORY DUMP & CIPHER STREAM ────────────────────────────────────┤%s\n",
                   t.primary, t.reset);

            // Animate memory hex dumps
            for (int r = 0; r < 4; r++) {
                printf("%s│ %s0x%08X : %02X %02X %02X %02X %02X %02X %02X %02X  | %c%c%c%c%c%c%c%c %s│%s\n",
                       t.primary, t.text,
                       (unsigned int)(0x7FFF0000 + (rand() % 0xFFFF)),
                       rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                       rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                       'A' + (rand() % 26), 'a' + (rand() % 26), '0' + (rand() % 10),
                       '#' + (rand() % 5), 'A' + (rand() % 26), '!', '*', 'X',
                       t.primary, t.reset);
            }

            // Overall Progress
            int percent = (p * 100 + frame * 5) / num_ports;
            if (percent > 100) percent = 100;
            printf("%s└── INTRUSION PROGRESS: [%s", t.primary, t.highlight);
            int fill = (percent * 30) / 100;
            for (int k = 0; k < 30; k++) {
                printf("%s", k < fill ? "=" : " ");
            }
            printf("%s] %3d%% ──┘%s\n", t.primary, percent, t.reset);
            fflush(stdout);

            usleep(25000); // 25ms frame update
        }
    }

    // Stage 2: Final Movie-Style Screen Takeover Banner
    clear_screen();
    printf("\n\n");
    printf("%s  ╔══════════════════════════════════════════════════════════════╗%s\n", t.highlight, t.reset);
    printf("%s  ║  %s[ SYSTEM INTRUSION COMPLETE // ACCESS GRANTED ]%s           ║%s\n", t.highlight, t.primary, t.highlight, t.reset);
    printf("%s  ║                                                              ║%s\n", t.highlight, t.reset);
    printf("%s  ║  %sTARGET NODE    : %-40s%s ║%s\n", t.highlight, t.text, tgt, t.highlight, t.reset);
    printf("%s  ║  %sSECURITY LEVEL : BYPASSED (ROOT LEVEL 0)%s                 ║%s\n", t.highlight, t.text, t.highlight, t.reset);
    printf("%s  ║  %sEXPLOITED PORTS: 21, 22, 80, 443, 3000, 3306, 5432, 8080%s    ║%s\n", t.highlight, t.text, t.highlight, t.reset);
    printf("%s  ║  %sCIPHER KEY     : 0x98F3A0294CBA12E78801AAFF9321%s          ║%s\n", t.highlight, t.text, t.highlight, t.reset);
    printf("%s  ║  %sVULNERABILITY  : SURFACE MAPPED & LOGGED%s                 ║%s\n", t.highlight, t.text, t.highlight, t.reset);
    printf("%s  ╚══════════════════════════════════════════════════════════════╝%s\n\n", t.highlight, t.reset);
    fflush(stdout);

    usleep(2000000); // Hold final banner for 2 seconds

    printf("\033[?25h"); // Restore cursor
    clear_screen();
    print_menu_commands();
}

void print_sysinfo(void) {
    Theme t = get_current_theme();
    struct sysinfo info;
    sysinfo(&info);

    char hostname[128];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strncpy(hostname, "cell-host", sizeof(hostname) - 1);
        hostname[sizeof(hostname) - 1] = '\0';
    }

    printf("\n%s  ____ _____ _     _      %s  Host: %s%s\n", t.primary, t.secondary, hostname, t.reset);
    printf("%s / ___| ____| |   | |     %s  OS: Linux x86_64\n", t.primary, t.secondary);
    printf("%s| |   |  _| | |   | |     %s  Kernel: CELL Cyber-Kernel\n", t.primary, t.secondary);
    printf("%s| |___| |___| |___| |___  %s  Uptime: %ld mins\n", t.primary, t.secondary, info.uptime / 60);
    printf("%s \\____|_____|_____|_____| %s  Shell PID: %d\n", t.primary, t.secondary, getpid());
    printf("%s                          %s  Total RAM: %ld MB\n", t.primary, t.secondary, info.totalram / (1024 * 1024));
    printf("%s                          %s  Free RAM:  %ld MB\n\n", t.primary, t.secondary, info.freeram / (1024 * 1024));
}

void print_menu_commands(void) {
    Theme t = get_current_theme();
    printf("\n");
    printf("%s  help - all commands%s\n", t.primary, t.reset);
    printf("%s  theme - change themes%s\n", t.primary, t.reset);
    printf("%s  matrix - weird shit!%s\n", t.primary, t.reset);
    // printf("%s  scan - hacker shit!%s\n", t.primary, t.reset);
    printf("%s  exit - exit cell%s\n", t.primary, t.reset);
    printf("\n");
}

void print_boot_banner(void) {
    clear_screen();
    Theme t = get_current_theme();
    printf("\n\n");
    printf("%s", t.primary);
    printf("   ██████╗███████╗██╗     ██╗     \n");
    printf("  ██╔════╝██╔════╝██║     ██║     \n");
    printf("  ██║     █████╗  ██║     ██║     \n");
    printf("  ██║     ██╔══╝  ██║     ██║     \n");
    printf("  ╚██████╗███████╗███████╗███████╗\n");
    printf("   ╚═════╝╚══════╝╚══════╝╚══════╝\n");
    print_menu_commands();
}

void print_help(void) {
    Theme t = get_current_theme();
    printf("\n");
    printf("  %scd%s - change working directory\n", t.primary, t.reset);
    printf("  %spwd%s - print working directory\n", t.primary, t.reset);
    printf("  %scat%s - view file content\n", t.primary, t.reset);
    printf("  %smkdir%s - create directory\n", t.primary, t.reset);
    printf("  %srm%s - remove file or directory\n", t.primary, t.reset);
    printf("  %scp%s - copy file or directory\n", t.primary, t.reset);
    printf("  %smv%s - move or rename file\n", t.primary, t.reset);
    printf("  %sgrep%s - search pattern in files\n", t.primary, t.reset);
    printf("  %schmod%s - change file permissions\n\n", t.primary, t.reset);
}
