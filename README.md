# CHRONO

CHRONO is a retro 16-bit pixel-art terminal emulator and Unix shell built with C, Rust (Tauri v2), and xterm.js.

## System Architecture

- Core Shell Engine: Written in C (`cell.c`, `ui.c`). Handles command execution, PTY streaming, process control, and readline integration.
- Desktop Shell Host: Built with Rust (Tauri v2) and xterm.js frontend. Provides frameless window management, custom title bar drag handles, and PTY I/O communication over IPC.

## Key Features

- Startup Directory: Launches directly in user home directory (`~`).
- Dynamic Prompt: Path format displays as `{user} ~` and subpaths relative to root home directory.
- Theme Switcher: Built-in `theme` command supporting live switching across 5 retro pixel-art themes (`sunset`, `cyberpunk`, `forest`, `synthwave`, `matrix`).
- Text Contrast Layer: Left-side 60% black gradient overlay ensuring high legibility over pixel backgrounds.
- Smart Clipboard Handling: `Ctrl+C` copies selected text if highlighted; sends SIGINT interrupt if no selection exists. `Ctrl+V` pastes clipboard contents without duplication.
- Window Management: Frameless rounded window (`12px` border-radius) with custom minimize, maximize, and close controls.

## Prerequisites

- GCC / Clang
- libreadline-dev
- Rust & Cargo
- Node.js & npm

## Building & Execution

### 1. Build Core Shell Binary
```bash
make clean && make
```

### 2. Run Desktop Terminal Application
```bash
cd cell-terminal
npm install
npm run tauri dev
```

### 3. Production Application Bundle
```bash
cd cell-terminal
npm run tauri build
```

## Built-In Commands

- `theme <name>`: Switch visual theme (`sunset`, `cyberpunk`, `forest`, `synthwave`, `matrix`).
- `sysinfo`: Display system specifications and hardware details.
- `matrix`: Run retro digital code rain animation.
- `scan <target>`: Run network security port scanner simulation.
- `cd <path>`: Change working directory.
- `help`: Print available commands.
