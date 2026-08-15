#ifndef UI_H
#define UI_H

// Color constants for prompt and UI output
#define COLOR_PRIMARY   "\033[1;36m" // Cyan
#define COLOR_SECONDARY "\033[0;36m" // Muted Cyan
#define COLOR_HIGHLIGHT "\033[1;33m" // Yellow / Accent
#define COLOR_TEXT      "\033[0m"    // White / Default
#define COLOR_RESET     "\033[0m"

// Screen functions
int get_terminal_width(void);
void clear_screen(void);

// Visual FX & Animations
void run_matrix_animation(int seconds);
void run_hacker_scanner(const char *target);

// Information & Help Displays
void print_boot_banner(void);
void print_menu_commands(void);
void print_sysinfo(void);
void print_help(void);

#endif // UI_H
