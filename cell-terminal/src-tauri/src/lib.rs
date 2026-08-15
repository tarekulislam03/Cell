use portable_pty::{native_pty_system, CommandBuilder, PtySize};
use serde::Deserialize;
use std::io::{Read, Write};
use std::path::PathBuf;
use std::sync::{Arc, Mutex};
use std::thread;
use tauri::{AppHandle, Emitter, State, Window};

// State container holding the PTY writer and master handle
struct PtyState {
    writer: Arc<Mutex<Option<Box<dyn Write + Send>>>>,
    master: Arc<Mutex<Option<Box<dyn portable_pty::MasterPty + Send>>>>,
}

#[derive(Clone, serde::Serialize)]
struct PtyOutput {
    data: String,
}

#[derive(Deserialize)]
struct ResizePayload {
    rows: u16,
    cols: u16,
}

// Window Management Commands
#[tauri::command]
fn minimize_window(window: Window) {
    let _ = window.minimize();
}

#[tauri::command]
fn toggle_maximize_window(window: Window) {
    if let Ok(is_max) = window.is_maximized() {
        if is_max {
            let _ = window.unmaximize();
        } else {
            let _ = window.maximize();
        }
    }
}

#[tauri::command]
fn close_window(window: Window) {
    let _ = window.close();
}

#[tauri::command]
fn start_drag_window(window: Window) {
    let _ = window.start_dragging();
}

// Helper to locate the compiled C 'cell' shell binary
fn find_cell_executable() -> PathBuf {
    if let Ok(manifest_dir) = std::env::var("CARGO_MANIFEST_DIR") {
        let path = PathBuf::from(manifest_dir).join("../cell");
        if path.exists() {
            return path.canonicalize().unwrap_or(path);
        }
    }

    if let Ok(cwd) = std::env::current_dir() {
        let path = cwd.join("../cell");
        if path.exists() {
            return path.canonicalize().unwrap_or(path);
        }
        let direct_path = cwd.join("cell");
        if direct_path.exists() {
            return direct_path.canonicalize().unwrap_or(direct_path);
        }
    }

    let default_path = PathBuf::from("/home/tarekul/Documents/job & intership/projects/Unix Shell/cell");
    if default_path.exists() {
        return default_path;
    }

    PathBuf::from("cell")
}

// Tauri Command: Spawn the C shell inside a native PTY
#[tauri::command]
fn spawn_shell(app: AppHandle, state: State<'_, PtyState>) -> Result<(), String> {
    let pty_system = native_pty_system();

    let pair = pty_system
        .openpty(PtySize {
            rows: 24,
            cols: 80,
            pixel_width: 0,
            pixel_height: 0,
        })
        .map_err(|e| format!("Failed to open PTY: {}", e))?;

    let cell_bin = find_cell_executable();
    println!("[CELL Terminal] Spawning shell executable: {:?}", cell_bin);

    let mut cmd = CommandBuilder::new(&cell_bin);
    if let Ok(home) = std::env::var("HOME") {
        cmd.cwd(home);
    } else if let Some(parent) = cell_bin.parent() {
        cmd.cwd(parent);
    }

    cmd.env("TERM", "xterm-256color");
    cmd.env("COLORTERM", "truecolor");

    let _child = pair
        .slave
        .spawn_command(cmd)
        .map_err(|e| format!("Failed to spawn shell '{:?}': {}", cell_bin, e))?;

    let mut reader = pair
        .master
        .try_clone_reader()
        .map_err(|e| format!("Failed to clone PTY reader: {}", e))?;

    let writer = pair
        .master
        .take_writer()
        .map_err(|e| format!("Failed to take PTY writer: {}", e))?;

    *state.writer.lock().unwrap() = Some(writer);
    *state.master.lock().unwrap() = Some(pair.master);

    let writer_clone = Arc::clone(&state.writer);

    // Spawn reader thread: reads PTY output and emits to frontend
    thread::spawn(move || {
        let mut buf = [0u8; 4096];
        loop {
            match reader.read(&mut buf) {
                Ok(0) => {
                    println!("[CELL PTY] Reader received EOF");
                    break;
                }
                Ok(n) => {
                    let data = String::from_utf8_lossy(&buf[..n]).to_string();
                    let _ = app.emit("pty-output", PtyOutput { data });
                }
                Err(e) => {
                    eprintln!("[CELL PTY] Reader error: {:?}", e);
                    break;
                }
            }
        }
        let _ = writer_clone.lock().unwrap().take();
        let _ = app.emit("pty-exit", ());
        std::thread::sleep(std::time::Duration::from_millis(500));
        app.exit(0);
    });

    Ok(())
}

// Tauri Command: Write input bytes to the PTY
#[tauri::command]
fn write_to_pty(input: String, state: State<'_, PtyState>) -> Result<(), String> {
    if let Some(ref mut writer) = *state.writer.lock().unwrap() {
        writer
            .write_all(input.as_bytes())
            .map_err(|e| e.to_string())?;
        writer.flush().map_err(|e| e.to_string())?;
    }
    Ok(())
}

// Tauri Command: Resize the PTY
#[tauri::command]
fn resize_pty(payload: ResizePayload, state: State<'_, PtyState>) -> Result<(), String> {
    if let Some(ref master) = *state.master.lock().unwrap() {
        master
            .resize(PtySize {
                rows: payload.rows,
                cols: payload.cols,
                pixel_width: 0,
                pixel_height: 0,
            })
            .map_err(|e| e.to_string())?;
    }
    Ok(())
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_store::Builder::default().build())
        .manage(PtyState {
            writer: Arc::new(Mutex::new(None)),
            master: Arc::new(Mutex::new(None)),
        })
        .invoke_handler(tauri::generate_handler![
            spawn_shell,
            write_to_pty,
            resize_pty,
            minimize_window,
            toggle_maximize_window,
            close_window,
            start_drag_window
        ])
        .run(tauri::generate_context!())
        .expect("error while running CELL Terminal");
}
