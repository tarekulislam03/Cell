#ifndef UI_H
#define UI_H

typedef struct {
    const char *name;
    const char *primary;    // Prompt & Headers
    const char *secondary;  // Accents & Borders
    const char *highlight;  // Warnings / Special
    const char *text;       // High contrast readable text
    const char *reset;      // Reset color
} Theme;

// Theme management
Theme get_current_theme(void);
int set_theme(const char *name);
void print_available_themes(void);

// Screen functions
int get_terminal_width(void);
void clear_screen(void);

// Visual FX & Hacker Animations
void run_matrix_animation(int seconds);
void run_hacker_scanner(const char *target);

// Information & Help Displays
void print_boot_banner(void);
void print_menu_commands(void);
void print_sysinfo(void);
void print_help(void);

#endif // UI_H
