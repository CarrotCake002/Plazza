I am working on a C++ project called "Plazza" (an advanced multithreaded/multi-process simulation). I want to build a modern visual frontend for it using a React web application wrapped in Tauri, while keeping the C++ backend completely decoupled.

Here are the strict architecture guidelines and specifications you must follow:

### 1. Modularity & Independent Execution Constraints
- The C++ backend must remain 100% functional on its own without the frontend.
- The frontend must be able to launch independently. If the C++ backend is not running, the UI should gracefully display a "Disconnected" or "Backend Offline" state.
- Do NOT merge the frontend and backend build steps. Keep them in separate directories (e.g., `/backend` and `/frontend`).

### 2. Backend Specifications (C++)
- We will use the Crow web framework (header-only, lightweight) to turn the Plazza engine into a local server.
- The web server must be launched via an optional trailing command-line argument.

The current required format for the launch is:
Usage: ./plazza [speed_multi] [cook_nb] [restock_time]
Where:
- [speed_multi]: Pizza cooking time multiplier. It takes any positive value, where values between 0 and 1 will increase the cooking time.
- [cook_nb]: The number of cooks per kitchen. This will determine the amount of orders every kitchen can accept.
- [restock_time]: This is the amount of time (in milliseconds) it will take to restock 1 unit of every ingredient in the ingredients stock.

To support the frontend, you must add an optional 4th argument for the network port:
- Standard command: `./plazza [speed_multi] [cook_nb] [restock_time]` -> Skips web initialization entirely; runs exactly as it does right now via the CLI.
- Frontend command: `./plazza [speed_multi] [cook_nb] [restock_time] [port]` -> Spawns the Crow server on a separate background thread listening on the specified port.

- Use `nlohmann/json` to serialize our internal Plazza state (e.g., active kitchens, cook status, ingredient levels, task queues) and expose it via a clean HTTP GET route like `/api/status`.

### 3. Frontend Specifications (React + Tauri)
- Create a modern, scannable dashboard using Vite + React + Tailwind CSS.
- Wrap this web view inside Tauri so it can run as a lightweight native desktop window.
- The frontend must use client-side polling or a fetch loop targeting `http://localhost:[PORT]/api/status` to visually map out our Plazza kitchens, cooks, and ingredient levels in real-time.

---

## Current Implementation Status

### Backend (C++)

#### Build & Run
```bash
cmake -B build && cmake --build build

# CLI-only mode (no web server)
./build/plazza [speed_multi] [cook_nb] [restock_time]

# Server mode (CLI + Crow HTTP server)
./build/plazza [speed_multi] [cook_nb] [restock_time] [port]
```

#### Dependencies (auto-fetched by CMake)
- **Crow** (v1.2.0) — header-only C++ HTTP framework
- **ASIO** (standalone, asio-1-28-0) — async I/O for Crow
- **nlohmann/json** (v3.11.3) — JSON serialization

#### `/api/status` Endpoint
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

#### Architecture: Shared Memory for Child→Parent Status
Kitchen processes are forked children. To expose per-kitchen state (`orders_queued`, `orders_cooking`, `cooks_total`) in `/api/status`, a shared memory back-channel was added:

1. **`include/KitchenStatus.hpp`** — struct with `std::atomic<int>` fields in an `mmap`'d `MAP_SHARED | MAP_ANONYMOUS` page
2. Before `fork()`, `Reception::allocSharedStatus()` maps the page; both parent and child inherit direct access
3. `Kitchen::addOrderToList()` and `Kitchen::run_cook()` write inline atomic stores after state changes
4. `/api/status` handler reads the fields with `memory_order_relaxed` — eventually consistent, zero-overhead

No pipes, no threads, no files. One syscall per kitchen at creation.

#### Key Files Modified
| File | Change |
|------|--------|
| `main.cpp` | Optional 4th arg → `runWithServer()`. Crow `SimpleApp` with `/api/status`. Saved `std::future` from `run_async()`. Enriched JSON with kitchen status + order details. |
| `include/Reception.hpp` | `KitchenInfo` gets `KitchenStatus* status`. New `allocSharedStatus()`/`freeSharedStatus()`. Thread-safe getters with `std::mutex`. |
| `src/Reception.cpp` | mmap alloc/free in `createNewKitchen()`/`shutdown()`. EOF check in `getInput()`. Lock guards around shared vector mutations. |
| `include/Kitchen.hpp` | Constructor accepts `KitchenStatus*`. Stores as `status_shm` member. |
| `src/Kitchen.cpp` | Inline `std::atomic` stores in `addOrderToList()` and `run_cook()` after state changes. |
| `include/KitchenStatus.hpp` | **New file.** Shared-memory struct: `orders_queued`, `orders_cooking`, `cooks_total`. |
| `include/usage.hpp` | Updated `HELP` string with optional `[port]` argument. |
| `src/usage.cpp` | `errorHandling()` accepts 3 or 4 args (`ac == 4 || ac == 5`). Validates port range 1–65535. |
| `CMakeLists.txt` | `FetchContent` for Crow, ASIO, nlohmann/json. Linked `Threads::Threads`. |

### Frontend (React + Vite + Tailwind + Tauri)

#### Quick Start
```bash
cd frontend
npm install
npm run dev          # Vite dev server at http://localhost:5173
```

The Vite dev server proxies `/api/*` → `http://127.0.0.1:8080` so no CORS issues in development. Start the C++ backend first, then the frontend.

#### Production Build
```bash
cd frontend
npm run build        # outputs to frontend/dist/
```

#### Tauri Native Desktop Wrapper
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

#### Tauri Scaffold (`frontend/src-tauri/`)
| File | Purpose |
|------|---------|
| `tauri.conf.json` | Window config (1024×768), app identifier `com.plazza.app`, points `devUrl` at Vite, builds from `dist/` |
| `Cargo.toml` | Rust deps: tauri 2.x, tauri-build, tauri-plugin-log, serde, serde_json |
| `src/main.rs` | Binary entry point |
| `src/lib.rs` | Tauri builder setup with log plugin in debug mode |

#### Dashboard (`frontend/src/`)
| File | Purpose |
|------|---------|
| `App.jsx` | Polls `/api/status` every 2s. Shows config cards, per-kitchen stats (queued/cooking/cooks), order detail pills, offline/connecting/error states |
| `index.css` | Tailwind CSS v4 import |
| `main.jsx` | React entry point |
| `vite.config.js` | React + Tailwind plugins, dev proxy `/api` → `127.0.0.1:8080` |

#### Dashboard States
- **Connected** (green dot) — backend responding, live stats visible
- **Backend Offline** (red dot) — backend unreachable, shows startup command hint
- **Error** (yellow dot) — transient HTTP error, retries on next poll
- **Empty kitchens** — dashed placeholder: "No kitchens active. Send an order to spawn one."
- **Empty orders** — "None" label under New/Pending sections

### Tests
```bash
cd build
./plazza_tests       # 47 tests, all passing
```

### Known Gaps
- **No ingredient/stock tracking** — `restock_timer` parameter exists but is unused
- **`pending` orders never cleared** — cooks move orders to `pending` but never remove them (no cooking timer simulation)
- **No `busy_cooks` counter** — cooks consume orders instantly (no timer), so busy/idle distinction is meaningless until cooking simulation is added
- **No kitchen death detection** — dead kitchen's last-known status values persist in shared memory. Needs `SIGCHLD` handler or `waitpid(WNOHANG)` loop
- **No CORS on Crow side** — Vite proxy handles it in dev; production build needs Crow CORS middleware
- **Placeholder Tauri icons** — replace `src-tauri/icons/` with real app icons before shipping
