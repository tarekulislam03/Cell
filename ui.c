#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <time.h>
#include "ui.h"

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

    time_t start_time = time(NULL);
    while (time(NULL) - start_time < seconds) {
        printf("\033[H");
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (drops[x] == y) {
                    printf("%s%c", COLOR_HIGHLIGHT, charset[rand() % charset_len]);
                } else if ((drops[x] - 1 + height) % height == y) {
                    printf("%s%c", COLOR_PRIMARY, charset[rand() % charset_len]);
                } else if ((drops[x] - 2 + height) % height == y) {
                    printf("%s%c", COLOR_SECONDARY, charset[rand() % charset_len]);
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
        usleep(50000);
    }

    free(drops);
    printf("\033[?25h"); // Show cursor
    clear_screen();
    print_menu_commands();
}

void run_hacker_scanner(const char *target) {
    const char *tgt = (target && strlen(target) > 0) ? target : "127.0.0.1 (LOCAL NODE)";

    clear_screen();
    printf("\033[?25l"); // Hide cursor

    int ports[] = {21, 22, 80, 443, 3000, 3306, 5432, 8080};
    const char *services[] = {"FTP", "SSH", "HTTP", "HTTPS", "DEV-NODE", "MYSQL", "POSTGRES", "HTTP-ALT"};
    int num_ports = sizeof(ports) / sizeof(ports[0]);

    for (int p = 0; p < num_ports; p++) {
        for (int frame = 0; frame <= 20; frame++) {
            printf("\033[H");

            printf("%s┌── SYSTEM INFILTRATION SCANNER // TARGET: %s%s%s ──┐%s\n",
                   COLOR_PRIMARY, COLOR_HIGHLIGHT, tgt, COLOR_PRIMARY, COLOR_RESET);
            printf("%s│%s MODE: FULL EXPLOIT MATRIX | THREADS: 32 | PROTOCOL: TCP/IP %s│%s\n",
                   COLOR_PRIMARY, COLOR_TEXT, COLOR_PRIMARY, COLOR_RESET);
            printf("%s├─── PORT VULNERABILITY SCANNER ───────────────────────────────────────────────┤%s\n",
                   COLOR_PRIMARY, COLOR_RESET);

            for (int i = 0; i < num_ports; i++) {
                if (i < p) {
                    printf("%s│ %s%-8s (PORT %4d) : %s[████████████████████] %s[ OPEN / EXPLOITED ] %s│%s\n",
                           COLOR_PRIMARY, COLOR_TEXT, services[i], ports[i], COLOR_PRIMARY, COLOR_HIGHLIGHT, COLOR_PRIMARY, COLOR_RESET);
                } else if (i == p) {
                    char bar[21];
                    for (int b = 0; b < 20; b++) {
                        bar[b] = (b < frame) ? '#' : '.';
                    }
                    bar[20] = '\0';
                    printf("%s│ %s%-8s (PORT %4d) : %s[%-20s] %s[ BYPASSING... ]   %s│%s\n",
                           COLOR_PRIMARY, COLOR_TEXT, services[i], ports[i], COLOR_HIGHLIGHT, bar, COLOR_SECONDARY, COLOR_PRIMARY, COLOR_RESET);
                } else {
                    printf("%s│ %s%-8s (PORT %4d) : %s[....................] %s[ WAITING... ]      %s│%s\n",
                           COLOR_PRIMARY, COLOR_TEXT, services[i], ports[i], COLOR_SECONDARY, COLOR_SECONDARY, COLOR_PRIMARY, COLOR_RESET);
                }
            }

            printf("%s├─── REALTIME MEMORY DUMP & CIPHER STREAM ────────────────────────────────────┤%s\n",
                   COLOR_PRIMARY, COLOR_RESET);

            for (int r = 0; r < 4; r++) {
                printf("%s│ %s0x%08X : %02X %02X %02X %02X %02X %02X %02X %02X  | %c%c%c%c%c%c%c%c %s│%s\n",
                       COLOR_PRIMARY, COLOR_TEXT,
                       (unsigned int)(0x7FFF0000 + (rand() % 0xFFFF)),
                       rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                       rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                       'A' + (rand() % 26), 'a' + (rand() % 26), '0' + (rand() % 10),
                       '#' + (rand() % 5), 'A' + (rand() % 26), '!', '*', 'X',
                       COLOR_PRIMARY, COLOR_RESET);
            }

            int percent = (p * 100 + frame * 5) / num_ports;
            if (percent > 100) percent = 100;
            printf("%s└── INTRUSION PROGRESS: [%s", COLOR_PRIMARY, COLOR_HIGHLIGHT);
            int fill = (percent * 30) / 100;
            for (int k = 0; k < 30; k++) {
                printf("%s", k < fill ? "=" : " ");
            }
            printf("%s] %3d%% ──┘%s\n", COLOR_PRIMARY, percent, COLOR_RESET);
            fflush(stdout);

            usleep(25000);
        }
    }

    clear_screen();
    printf("\n\n");
    printf("%s  ╔══════════════════════════════════════════════════════════════╗%s\n", COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ║  %s[ SYSTEM INTRUSION COMPLETE // ACCESS GRANTED ]%s           ║%s\n", COLOR_HIGHLIGHT, COLOR_PRIMARY, COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ║                                                              ║%s\n", COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ║  %sTARGET NODE    : %-40s%s ║%s\n", COLOR_HIGHLIGHT, COLOR_TEXT, tgt, COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ║  %sSECURITY LEVEL : BYPASSED (ROOT LEVEL 0)%s                 ║%s\n", COLOR_HIGHLIGHT, COLOR_TEXT, COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ║  %sEXPLOITED PORTS: 21, 22, 80, 443, 3000, 3306, 5432, 8080%s    ║%s\n", COLOR_HIGHLIGHT, COLOR_TEXT, COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ║  %sCIPHER KEY     : 0x98F3A0294CBA12E78801AAFF9321%s          ║%s\n", COLOR_HIGHLIGHT, COLOR_TEXT, COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ║  %sVULNERABILITY  : SURFACE MAPPED & LOGGED%s                 ║%s\n", COLOR_HIGHLIGHT, COLOR_TEXT, COLOR_HIGHLIGHT, COLOR_RESET);
    printf("%s  ╚══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_HIGHLIGHT, COLOR_RESET);
    fflush(stdout);

    usleep(2000000);

    printf("\033[?25h");
    clear_screen();
    print_menu_commands();
}

void print_sysinfo(void) {
    struct sysinfo info;
    sysinfo(&info);

    char hostname[128];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strncpy(hostname, "cell-host", sizeof(hostname) - 1);
        hostname[sizeof(hostname) - 1] = '\0';
    }

    printf("\n%s  ____ _____ _     _      %s  Host: %s%s\n", COLOR_PRIMARY, COLOR_SECONDARY, hostname, COLOR_RESET);
    printf("%s / ___| ____| |   | |     %s  OS: Linux x86_64\n", COLOR_PRIMARY, COLOR_SECONDARY);
    printf("%s| |   |  _| | |   | |     %s  Kernel: CELL Cyber-Kernel\n", COLOR_PRIMARY, COLOR_SECONDARY);
    printf("%s| |___| |___| |___| |___  %s  Uptime: %ld mins\n", COLOR_PRIMARY, COLOR_SECONDARY, info.uptime / 60);
    printf("%s \\____|_____|_____|_____| %s  Shell PID: %d\n", COLOR_PRIMARY, COLOR_SECONDARY, getpid());
    printf("%s                          %s  Total RAM: %ld MB\n", COLOR_PRIMARY, COLOR_SECONDARY, info.totalram / (1024 * 1024));
    printf("%s                          %s  Free RAM:  %ld MB\n\n", COLOR_PRIMARY, COLOR_SECONDARY, info.freeram / (1024 * 1024));
    fflush(stdout);
}

void print_menu_commands(void) {
    printf("\n");
    printf("%s  dev & system commands:%s\n", COLOR_PRIMARY, COLOR_RESET);
    printf("    %scd <dir>%s           change directory\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %spwd%s                print current directory path\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %sls / find%s          list / search files in directory\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %scat / grep%s         view file content / search text pattern\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %smkdir / rm / cp / mv%s file & directory operations\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %sgit <cmd>%s          version control operations\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %ssysinfo%s            display system hardware & kernel specs\n", COLOR_SECONDARY, COLOR_RESET);
    printf("\n");
    printf("%s  customization commands:%s\n", COLOR_PRIMARY, COLOR_RESET);
    printf("    %sset opacity <val>%s    bg opacity (0.0 to 0.4)\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %sset blur <px>%s        bg blur in pixels (0 to 40)\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %sset font <px>%s        font size (10 to 28)\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %sset cursor <style>%s   cursor style (bar, block, underline)\n", COLOR_SECONDARY, COLOR_RESET);
    printf("\n");
    printf("%s  utilities:%s\n", COLOR_PRIMARY, COLOR_RESET);
    printf("    %smatrix / scan%s       visual rain & security port scan simulation\n", COLOR_SECONDARY, COLOR_RESET);
    printf("    %sclear / exit%s        clear terminal / exit shell\n\n", COLOR_SECONDARY, COLOR_RESET);
    fflush(stdout);
}

void print_boot_banner(void) {
    clear_screen();
    fflush(stdout);
}

void print_help(void) {
    print_menu_commands();
}
