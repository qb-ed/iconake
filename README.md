# iconake

**Snake, played with your Windows desktop icons.** The snake *is* your icons: they glide across the desktop, eat a wobbling food icon, and grow. When the game ends, your layout is put back exactly as it was.

Pure C. One file. No CRT. Nothing but the OS itself. Tested on Windows 11.

<!-- ![demo](demo.gif) — add a ScreenToGif recording here, it's worth 10x the text -->

## Features
- Classic snake on a grid derived from your real icon spacing (`IFolderView::GetSpacing`) and work area
- Batched icon moves via `IFolderView::SelectAndPositionItems`, wrapped in
  `IFolderView2::SetRedraw(FALSE/TRUE)` — no flicker, no OneDrive/shell-extension stutter
- Crash-safe: layout backed up to `%TEMP%\iconake.layout` *before anything moves*;
  `--restore` puts it back even after a hard kill, power loss, or Explorer restart
- Unused icons parked off-screen (`--no-park` to leave them in place)
- Low-level keyboard hook (arrows/WASD, Esc quits) or `--no-hook` polling fallback
- 1 ms timer resolution while playing, so the 150 ms tick doesn't beat against the scheduler
- CRT-free build (`-nostdlib`): tiny binary, instant start
- Handles Win10 19H1+/Win11 desktop topology (Progman / WorkerW / SHELLDLL_DefView)

## Usage
```
iconake.exe              play
iconake.exe --restore    restore the layout backed up before the last run
iconake.exe --no-hook    poll GetAsyncKeyState instead of the global hook
iconake.exe --no-park    leave unused icons where they are
```
Requirements: Windows 10 19H1+ / Windows 11, at least 5 desktop icons.

## Build
```
clang iconake.c -o iconake.exe -Os -flto -s -ffreestanding -fno-builtin -fno-stack-protector -mno-stack-arg-probe -fno-asynchronous-unwind-tables -fno-ident -ffunction-sections -fdata-sections -nostdlib -Wl,--entry,entry -Wl,--subsystem,windows -Wl,--gc-sections -lkernel32 -luser32 -lole32 -lwinmm
```

## How it works
1. `IShellWindows::FindWindowSW(SWC_DESKTOP)` → `IServiceProvider` →
   `SID_STopLevelBrowser` → `IShellBrowser::QueryActiveShellView` → `IFolderView`/`IFolderView2`.
2. Snapshot every icon's PIDL + position; write the versioned binary backup (magic `ICKN`).
3. Clear `FWF_SNAPTOGRID` for the session; compute cols/rows from spacing + work area.
4. Each tick: `SetRedraw(FALSE)` → **one** `SelectAndPositionItems` call for all segments +
   food → `SetRedraw(TRUE)` → invalidate. No per-icon messages, no `Refresh()` in the loop.
5. On win/lose/quit: restore positions, restore folder flags, delete the backup.

## Safety
- No injection into Explorer, no cross-process `VirtualAllocEx`/`WriteProcessMemory`,
  no `LVM_*` message hacks — supported COM interfaces only.
- The program refuses to start if it cannot write the backup first.
- If it is killed mid-game, icons stay displaced until `iconake.exe --restore`.

## Limits
- Max 512 icons, max snake length 256.
- Playfield is the primary monitor's work area; spare icons park across the virtual screen.

## License
MIT
