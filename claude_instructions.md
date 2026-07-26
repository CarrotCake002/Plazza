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
