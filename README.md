# Plazza

Recreation of the Plazza project I did at EPITECH — an advanced multithreaded/multi-process pizza kitchen simulation with a React + Tauri dashboard.

## Backend (C++)

### Build & Run
```bash
cmake -B build && cmake --build build

# CLI-only mode (no web server)
./build/plazza [speed_multi] [cook_nb] [restock_time]

# Server mode (CLI + Crow HTTP server)
./build/plazza [speed_multi] [cook_nb] [restock_time] [port]
```

### Dependencies (auto-fetched by CMake)
- **Crow** (v1.2.0) — header-only C++ HTTP framework
- **ASIO** (standalone, asio-1-28-0) — async I/O for Crow
- **nlohmann/json** (v3.11.3) — JSON serialization

### `/api/status` Endpoint
Returns JSON with live Plazza state. Only available in server mode (when `[port]` is provided).

```json
{
  "config": {
    "speed_multiplier": 1.0,
    "cooks_per_kitchen": 3,
    "restock_timer_ms": 1000
  },
  "kitchens": [
    {
      "id": 0,
      "pid": 39738,
      "orders_queued": 2,
      "orders_cooking": 1,
      "cooks_total": 3
    }
  ],
  "orders": {
    "new": [{"type": "Regina", "size": "M", "amount": 2}],
    "pending": [{"type": "Fantasia", "size": "XL", "amount": 1}]
  }
}
```

### Architecture: Shared Memory for Child→Parent Status
Kitchen processes are forked children. To expose per-kitchen state (`orders_queued`, `orders_cooking`, `cooks_total`) in `/api/status`, a shared memory back-channel was added:

1. **`include/KitchenStatus.hpp`** — struct with `std::atomic<int>` fields in an `mmap`'d `MAP_SHARED | MAP_ANONYMOUS` page
2. Before `fork()`, `Reception::allocSharedStatus()` maps the page; both parent and child inherit direct access
3. `Kitchen::addOrderToList()` and `Kitchen::run_cook()` write inline atomic stores after state changes
4. `/api/status` handler reads the fields with `memory_order_relaxed` — eventually consistent, zero-overhead

No pipes, no threads, no files. One syscall per kitchen at creation.

### Key Files
| File | Purpose |
|------|--------|
| `main.cpp` | Optional 4th arg → `runWithServer()`. Crow `SimpleApp` with `/api/status`. |
| `include/Reception.hpp` | `KitchenInfo` gets `KitchenStatus* status`. Thread-safe getters with `std::mutex`. |
| `src/Reception.cpp` | mmap alloc/free in `createNewKitchen()`/`shutdown()`. EOF check in `getInput()`. |
| `include/Kitchen.hpp` | Constructor accepts `KitchenStatus*`. Stores as `status_shm` member. |
| `src/Kitchen.cpp` | Inline `std::atomic` stores in `addOrderToList()` and `run_cook()` after state changes. |
| `include/KitchenStatus.hpp` | **New file.** Shared-memory struct: `orders_queued`, `orders_cooking`, `cooks_total`. |
| `include/usage.hpp` | Updated `HELP` string with optional `[port]` argument. |
| `src/usage.cpp` | `errorHandling()` accepts 3 or 4 args (`ac == 4 \|\| ac == 5`). Validates port range 1–65535. |
| `CMakeLists.txt` | `FetchContent` for Crow, ASIO, nlohmann/json. Linked `Threads::Threads`. |

## Frontend (React + Vite + Tailwind + Tauri)

### Quick Start
```bash
cd frontend
npm install
npm run dev          # Vite dev server at http://localhost:5173
```

The Vite dev server proxies `/api/*` → `http://127.0.0.1:8080` so no CORS issues in development. Start the C++ backend first, then the frontend.

### Production Build
```bash
cd frontend
npm run build        # outputs to frontend/dist/
```

### Tauri Native Desktop Wrapper
```bash
# Install system dependencies (one-time, Ubuntu/Debian)
sudo apt install libwebkit2gtk-4.1-dev libjavascriptcoregtk-4.1-dev \
                 libsoup-3.0-dev libgtk-3-dev libayatana-appindicator3-dev librsvg2-dev

# Ensure Rust toolchain is active (rustup, not system package)
export PATH="$HOME/.cargo/bin:$PATH"

# Dev mode — opens native window + Vite hot-reload
cd frontend
cargo tauri dev

# Production build — creates .deb / .AppImage
cargo tauri build
```

### Tauri Scaffold (`frontend/src-tauri/`)
| File | Purpose |
|------|---------|
| `tauri.conf.json` | Window config (1024×768), app identifier `com.plazza.app`, points `devUrl` at Vite, builds from `dist/` |
| `Cargo.toml` | Rust deps: tauri 2.x, tauri-build, tauri-plugin-log, serde, serde_json |
| `src/main.rs` | Binary entry point |
| `src/lib.rs` | Tauri builder setup with log plugin in debug mode |

### Dashboard (`frontend/src/`)
| File | Purpose |
|------|---------|
| `App.jsx` | Polls `/api/status` every 2s. Shows config cards, per-kitchen stats (queued/cooking/cooks), order detail pills, offline/connecting/error states |
| `index.css` | Tailwind CSS v4 import |
| `main.jsx` | React entry point |
| `vite.config.js` | React + Tailwind plugins, dev proxy `/api` → `127.0.0.1:8080` |

### Dashboard States
- **Connected** (green dot) — backend responding, live stats visible
- **Backend Offline** (red dot) — backend unreachable, shows startup command hint
- **Error** (yellow dot) — transient HTTP error, retries on next poll
- **Empty kitchens** — dashed placeholder: "No kitchens active. Send an order to spawn one."
- **Empty orders** — "None" label under New/Pending sections

## Tests
```bash
# Backend (47 tests)
cd build
./plazza_tests

# Frontend (8 tests)
cd frontend
npm run test
```

## Known Gaps
- **No ingredient/stock tracking** — `restock_timer` parameter exists but is unused
- **`pending` orders never cleared** — cooks move orders to `pending` but never remove them (no cooking timer simulation)
- **No `busy_cooks` counter** — cooks consume orders instantly (no timer), so busy/idle distinction is meaningless until cooking simulation is added
- **No kitchen death detection** — dead kitchen's last-known status values persist in shared memory. Needs `SIGCHLD` handler or `waitpid(WNOHANG)` loop
- **No CORS on Crow side** — Vite proxy handles it in dev; production build needs Crow CORS middleware
- **Placeholder Tauri icons** — replace `src-tauri/icons/` with real app icons before shipping
