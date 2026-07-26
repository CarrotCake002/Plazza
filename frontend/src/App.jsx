import { useState, useEffect, useCallback } from "react";

const API_URL = "/api/status";

const POLL_MS = 2000;

function StatCard({ label, value, unit }) {
  return (
    <div className="rounded-lg border border-gray-700 bg-gray-800/50 p-4">
      <p className="text-sm text-gray-400">{label}</p>
      <p className="text-2xl font-semibold text-white">
        {value}
        {unit && <span className="ml-1 text-sm text-gray-400">{unit}</span>}
      </p>
    </div>
  );
}

function OrderList({ label, orders }) {
  return (
    <div>
      <p className="mb-2 text-sm font-medium text-gray-400">
        {label} ({orders.length})
      </p>
      {orders.length === 0 ? (
        <p className="text-sm text-gray-600">None</p>
      ) : (
        <div className="flex flex-wrap gap-2">
          {orders.map((o, i) => (
            <span
              key={i}
              className="rounded bg-gray-800 px-2 py-1 text-sm text-gray-300"
            >
              {o.type} {o.size} <span className="text-gray-500">x{o.amount}</span>
            </span>
          ))}
        </div>
      )}
    </div>
  );
}

export default function App() {
  const [status, setStatus] = useState(null);
  const [error, setError] = useState(null);
  const [offline, setOffline] = useState(false);

  const fetchStatus = useCallback(async () => {
    try {
      const res = await fetch(API_URL);
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      const data = await res.json();
      setStatus(data);
      setOffline(false);
      setError(null);
    } catch (e) {
      setError(e.message);
      setOffline(true);
    }
  }, []);

  useEffect(() => {
    fetchStatus();
    const interval = setInterval(fetchStatus, POLL_MS);
    return () => clearInterval(interval);
  }, [fetchStatus]);

  return (
    <div className="min-h-screen bg-gray-950 text-gray-200">
      <header className="border-b border-gray-800 bg-gray-900 px-8 py-4">
        <div className="mx-auto flex max-w-5xl items-center justify-between">
          <h1 className="text-xl font-bold text-white">Plazza</h1>
          <div className="flex items-center gap-3">
            <span
              className={`inline-block h-2 w-2 rounded-full ${
                offline ? "bg-red-500" : error ? "bg-yellow-500" : "bg-green-500"
              }`}
            />
            <span className="text-sm text-gray-400">
              {offline ? "Backend Offline" : error ? "Error" : "Connected"}
            </span>
          </div>
        </div>
      </header>

      <main className="mx-auto max-w-5xl px-8 py-8">
        {offline ? (
          <div className="flex flex-col items-center gap-4 py-24">
            <div className="text-6xl">🍕</div>
            <h2 className="text-2xl font-semibold text-white">Backend Offline</h2>
            <p className="text-gray-400">
              Start the Plazza server with{" "}
              <code className="rounded bg-gray-800 px-2 py-0.5 text-sm text-yellow-400">
                ./plazza 1.0 3 1000 8080
              </code>
            </p>
          </div>
        ) : !status ? (
          <div className="flex items-center justify-center py-24">
            <p className="text-gray-400">Connecting...</p>
          </div>
        ) : (
          <>
            {/* Config Bar */}
            <div className="mb-8 grid grid-cols-3 gap-4">
              <StatCard label="Speed Multiplier" value={`${status.config.speed_multiplier}x`} />
              <StatCard label="Cooks per Kitchen" value={status.config.cooks_per_kitchen} />
              <StatCard label="Restock Timer" value={status.config.restock_timer_ms} unit="ms" />
            </div>

            {/* Kitchens */}
            <section className="mb-8">
              <div className="mb-4 flex items-center gap-3">
                <h2 className="text-lg font-semibold text-white">Kitchens</h2>
                <span className="rounded-full bg-gray-800 px-2 py-0.5 text-sm text-gray-400">
                  {status.kitchens.length}
                </span>
              </div>
              {status.kitchens.length === 0 ? (
                <p className="rounded-lg border border-dashed border-gray-700 py-8 text-center text-gray-500">
                  No kitchens active. Send an order to spawn one.
                </p>
              ) : (
                <div className="grid gap-3 sm:grid-cols-2 lg:grid-cols-3">
                  {status.kitchens.map((k) => (
                    <div
                      key={k.id}
                      className="rounded-lg border border-gray-700 bg-gray-800/50 p-4"
                    >
                      <div className="mb-3 flex items-center justify-between">
                        <p className="text-sm font-medium text-white">
                          Kitchen #{k.id}
                        </p>
                        <span className="font-mono text-xs text-gray-500">
                          PID {k.pid}
                        </span>
                      </div>
                      <div className="grid grid-cols-3 gap-2 text-center">
                        <div>
                          <p className="text-lg font-semibold text-white">
                            {k.orders_queued}
                          </p>
                          <p className="text-xs text-gray-400">Queued</p>
                        </div>
                        <div>
                          <p className="text-lg font-semibold text-white">
                            {k.orders_cooking}
                          </p>
                          <p className="text-xs text-gray-400">Cooking</p>
                        </div>
                        <div>
                          <p className="text-lg font-semibold text-white">
                            {k.cooks_total}
                          </p>
                          <p className="text-xs text-gray-400">Cooks</p>
                        </div>
                      </div>
                    </div>
                  ))}
                </div>
              )}
            </section>

            {/* Orders */}
            <section className="space-y-4">
              <h2 className="text-lg font-semibold text-white">Orders</h2>
              <OrderList label="New" orders={status.orders.new} />
              <OrderList label="Pending" orders={status.orders.pending} />
            </section>
          </>
        )}
      </main>
    </div>
  );
}
