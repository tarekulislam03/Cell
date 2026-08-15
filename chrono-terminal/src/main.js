// ============================================================
// CELL Terminal — Retro Pixel Art Theme Frontend (xterm.js)
// ============================================================

import { Terminal } from 'https://cdn.jsdelivr.net/npm/@xterm/xterm@5.5.0/+esm';
import { FitAddon } from 'https://cdn.jsdelivr.net/npm/@xterm/addon-fit@0.10.0/+esm';

const { invoke } = window.__TAURI__.core;
const { listen } = window.__TAURI__.event;

// Retro Themes Map (5 pixel art themes with matching header & button color palettes)
const THEMES = {
  sunset: {
    bgImage: './pixel_bg.png',
    borderColor: '#c47335',
    borderMuted: '#4a280f',
    amberText: '#d97724',
    greenText: '#52b788',
    iconBg: 'linear-gradient(135deg, rgba(217, 119, 36, 0.4), rgba(196, 115, 53, 0.2))',
    btnClose: '#e74c3c',
    btnCloseBorder: '#b02818',
    btnMin: '#e67e22',
    btnMinBorder: '#b0550e',
    btnMax: '#f1c40f',
    btnMaxBorder: '#b89508',
    termTheme: {
      background: 'transparent',
      foreground: '#ffffff',
      cursor: '#ffd60a',
      green: '#00ff87',
      yellow: '#ffd60a',
      cyan: '#00f0ff',
    }
  },
  cyberpunk: {
    bgImage: './pixel_bg_cyberpunk.png',
    borderColor: '#ff2a85',
    borderMuted: '#5c0032',
    amberText: '#00f0ff',
    greenText: '#ff2a85',
    iconBg: 'linear-gradient(135deg, rgba(255, 42, 133, 0.4), rgba(112, 0, 255, 0.25))',
    btnClose: '#ff0055',
    btnCloseBorder: '#b8003d',
    btnMin: '#7000ff',
    btnMinBorder: '#4f00b8',
    btnMax: '#00f0ff',
    btnMaxBorder: '#00a5b0',
    termTheme: {
      background: 'transparent',
      foreground: '#ffffff',
      cursor: '#ff2a85',
      green: '#00f0ff',
      yellow: '#ff2a85',
      cyan: '#bd00ff',
    }
  },
  forest: {
    bgImage: './pixel_bg_forest.png',
    borderColor: '#2d6a4f',
    borderMuted: '#1b4332',
    amberText: '#00ff9d',
    greenText: '#e0c354',
    iconBg: 'linear-gradient(135deg, rgba(45, 106, 79, 0.5), rgba(0, 255, 157, 0.2))',
    btnClose: '#e63946',
    btnCloseBorder: '#ab1d28',
    btnMin: '#e0c354',
    btnMinBorder: '#a68f35',
    btnMax: '#2a9d8f',
    btnMaxBorder: '#1a685e',
    termTheme: {
      background: 'transparent',
      foreground: '#ffffff',
      cursor: '#00ff9d',
      green: '#00ff9d',
      yellow: '#e0c354',
      cyan: '#38bdf8',
    }
  },
  synthwave: {
    bgImage: './pixel_bg_synthwave.png',
    borderColor: '#ff007f',
    borderMuted: '#4a0026',
    amberText: '#ff7700',
    greenText: '#00ffff',
    iconBg: 'linear-gradient(135deg, rgba(255, 0, 127, 0.4), rgba(255, 119, 0, 0.25))',
    btnClose: '#ff007f',
    btnCloseBorder: '#b8005b',
    btnMin: '#ff7700',
    btnMinBorder: '#b85500',
    btnMax: '#00ffff',
    btnMaxBorder: '#00b8b8',
    termTheme: {
      background: 'transparent',
      foreground: '#ffffff',
      cursor: '#ff7700',
      green: '#00ffff',
      yellow: '#ff7700',
      cyan: '#ff007f',
    }
  },
  matrix: {
    bgImage: './pixel_bg_matrix.png',
    borderColor: '#00ff41',
    borderMuted: '#0d2b1d',
    amberText: '#00ff41',
    greenText: '#50fa7b',
    iconBg: 'linear-gradient(135deg, rgba(0, 255, 65, 0.35), rgba(13, 43, 29, 0.4))',
    btnClose: '#ff2a4b',
    btnCloseBorder: '#b8152e',
    btnMin: '#00ff41',
    btnMinBorder: '#00b82e',
    btnMax: '#50fa7b',
    btnMaxBorder: '#32b857',
    termTheme: {
      background: 'transparent',
      foreground: '#ffffff',
      cursor: '#00ff41',
      green: '#00ff41',
      yellow: '#f1fa8c',
      cyan: '#50fa7b',
    }
  }
};

const DEFAULT_THEME = THEMES.sunset;

function applyTheme(themeName) {
  const themeKey = themeName.toLowerCase().trim();
  const theme = THEMES[themeKey];
  if (!theme) return;

  const bgLayer = document.getElementById('bg-layer');
  if (bgLayer) {
    bgLayer.style.backgroundImage = `url('${theme.bgImage}')`;
    bgLayer.style.opacity = '0.7';
  }

  document.documentElement.style.setProperty('--border-color', theme.borderColor);
  document.documentElement.style.setProperty('--border-muted', theme.borderMuted);
  document.documentElement.style.setProperty('--text-amber', theme.amberText);
  document.documentElement.style.setProperty('--text-green', theme.greenText);

  if (theme.iconBg) document.documentElement.style.setProperty('--icon-bg', theme.iconBg);
  if (theme.btnClose) document.documentElement.style.setProperty('--btn-close', theme.btnClose);
  if (theme.btnCloseBorder) document.documentElement.style.setProperty('--btn-close-border', theme.btnCloseBorder);
  if (theme.btnMin) document.documentElement.style.setProperty('--btn-min', theme.btnMin);
  if (theme.btnMinBorder) document.documentElement.style.setProperty('--btn-min-border', theme.btnMinBorder);
  if (theme.btnMax) document.documentElement.style.setProperty('--btn-max', theme.btnMax);
  if (theme.btnMaxBorder) document.documentElement.style.setProperty('--btn-max-border', theme.btnMaxBorder);

  if (window.term) {
    window.term.options.theme = {
      ...DEFAULT_THEME.termTheme,
      ...theme.termTheme
    };
  }
}

function initWindowControls() {
  document.getElementById('btn-min')?.addEventListener('click', (e) => {
    e.stopPropagation();
    invoke('minimize_window');
  });
  document.getElementById('btn-max')?.addEventListener('click', (e) => {
    e.stopPropagation();
    invoke('toggle_maximize_window');
  });
  document.getElementById('btn-close')?.addEventListener('click', (e) => {
    e.stopPropagation();
    invoke('close_window');
  });

  const titlebar = document.getElementById('titlebar');
  if (titlebar) {
    titlebar.addEventListener('mousedown', (e) => {
      if (e.button === 0 && !e.target.closest('#titlebar-controls')) {
        invoke('start_drag_window');
      }
    });
    titlebar.addEventListener('dblclick', (e) => {
      if (!e.target.closest('#titlebar-controls')) {
        invoke('toggle_maximize_window');
      }
    });
  }
}

async function init() {
  initWindowControls();

  const term = new Terminal({
    fontFamily: "'JetBrains Mono', monospace",
    fontSize: 16,
    letterSpacing: 0,
    fontWeight: '700',
    fontWeightBold: '700',
    cursorStyle: 'block',
    cursorBlink: true,
    allowTransparency: true,
    theme: DEFAULT_THEME.termTheme,
    scrollback: 5000,
    convertEol: true,
  });

  const fitAddon = new FitAddon();
  term.loadAddon(fitAddon);
  term.open(document.getElementById('terminal'));

  // Single DOM paste handler to prevent duplicate pasting
  document.addEventListener('paste', (event) => {
    event.preventDefault();
    const text = event.clipboardData?.getData('text');
    if (text) {
      invoke('write_to_pty', { input: text });
    }
  });

  // Smart Ctrl+C (Copy if text selected, Interrupt if not) & Ctrl+V (Paste)
  term.attachCustomKeyEventHandler((event) => {
    if ((event.ctrlKey || event.metaKey) && event.code === 'KeyC' && event.type === 'keydown') {
      const selection = term.getSelection();
      if (selection && selection.length > 0) {
        navigator.clipboard.writeText(selection);
        return false;
      }
      return true;
    }

    if ((event.ctrlKey || event.metaKey) && event.code === 'KeyV' && event.type === 'keydown') {
      return false;
    }

    return true;
  });

  window.term = term;
  window.fitAddon = fitAddon;
  fitAddon.fit();

  // Listen for PTY output & handle OSC theme switching
  await listen('pty-output', (event) => {
    const data = event.payload.data;
    const match = data.match(/\x1b\]777;theme;([a-zA-Z0-9_-]+)\x07/);
    if (match) {
      applyTheme(match[1]);
    }
    term.write(data);
  });

  // Spawn shell via Tauri IPC
  await invoke('spawn_shell');

  // Send terminal input to PTY
  term.onData((data) => {
    invoke('write_to_pty', { input: data });
  });

  // Handle resize
  term.onResize(({ cols, rows }) => {
    invoke('resize_pty', { payload: { rows, cols } });
  });

  window.addEventListener('resize', () => {
    fitAddon.fit();
  });

  term.focus();
}

init();
