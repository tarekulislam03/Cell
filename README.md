<div align="center">

<h1>CELL</h1>

<h3> Unix Shell Using C </h3>


<!-- badges -->

[![C Language](https://img.shields.io/badge/language-C99%20%2F%20C11-00599C?logo=c&logoColor=white)](https://en.cppreference.com/w/c)
[![GNU Readline](https://img.shields.io/badge/readline-GNU-339933?logo=gnu&logoColor=white)](https://gnu.org/software/readline/)
[![OS: Linux](https://img.shields.io/badge/os-linux%20x86__64-FCC624?logo=linux&logoColor=black)](#quick-start-guide)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](#license)

</div>

<br />

**CELL is a fast, lightweight Unix shell written in C featuring theme customization, quote-aware command tokenization, GNU Readline integration, and full-screen terminal animations.**

---

## Quick Start Guide

### Step 1: Prerequisites

Install `gcc`, `make`, and `libreadline-dev` on Linux (or Windows via WSL):

```bash
# Ubuntu / Debian / WSL (Windows Subsystem for Linux)
sudo apt update
sudo apt install build-essential libreadline-dev

# Arch Linux
sudo pacman -S base-devel readline

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install readline-devel
```

### Step 2: Build & Run

```bash
# Clone repository
git clone https://github.com/tarekulislam03/Cell.git
cd Cell

# Compile executable
make

# Launch CELL Shell
./cell
```
