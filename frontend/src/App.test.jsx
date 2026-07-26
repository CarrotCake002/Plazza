import { describe, it, expect, vi, beforeEach, afterEach } from "vitest";
import { render, screen } from "@testing-library/react";
import App from "./App";

const EMPTY = {
  config: { speed_multiplier: 1.0, cooks_per_kitchen: 3, restock_timer_ms: 1000 },
  kitchens: [{ id: 0, pid: 12345, orders_queued: 0, orders_cooking: 0, cooks_total: 3 }],
  orders: { new: [], pending: [] },
};

const BUSY = {
  ...EMPTY,
  kitchens: [{ id: 0, pid: 12345, orders_queued: 2, orders_cooking: 1, cooks_total: 3 }],
  orders: {
    new: [{ type: "Regina", size: "M", amount: 2 }],
    pending: [{ type: "Fantasia", size: "XL", amount: 1 }],
  },
};

function mockFetch(resp) {
  global.fetch = vi.fn(() =>
    Promise.resolve({
      ok: resp.ok ?? true,
      status: resp.status ?? 200,
      json: () => Promise.resolve(resp.data),
    })
  );
}

describe("App", () => {
  beforeEach(() => {
    global.fetch = undefined;
  });

  afterEach(() => {
    vi.restoreAllMocks();
    global.fetch = undefined;
  });

  it("shows connecting state initially", () => {
    mockFetch({ data: EMPTY });
    render(<App />);
    const el = screen.getAllByText("Connecting...");
    expect(el.length).toBeGreaterThanOrEqual(1);
  });

  it("renders config cards after fetch", async () => {
    mockFetch({ data: EMPTY });
    render(<App />);
    const cards = await screen.findAllByText("Speed Multiplier");
    expect(cards.length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText("Cooks per Kitchen").length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText("Restock Timer").length).toBeGreaterThanOrEqual(1);
  });

  it("renders kitchen card with stats", async () => {
    mockFetch({ data: BUSY });
    render(<App />);
    expect((await screen.findAllByText(/Kitchen #0/)).length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText("Queued").length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText("Cooking").length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText("Cooks").length).toBeGreaterThanOrEqual(1);
  });

  it("renders order detail pills", async () => {
    mockFetch({ data: BUSY });
    render(<App />);
    expect((await screen.findAllByText(/Regina M/)).length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText(/Fantasia XL/).length).toBeGreaterThanOrEqual(1);
  });

  it("shows empty kitchens placeholder when no kitchens", async () => {
    mockFetch({ data: { ...EMPTY, kitchens: [] } });
    render(<App />);
    expect((await screen.findAllByText(/No kitchens active/)).length).toBeGreaterThanOrEqual(1);
  });

  it("shows Backend Offline on HTTP error", async () => {
    mockFetch({ ok: false, status: 502, data: null });
    render(<App />);
    expect((await screen.findAllByText("Backend Offline")).length).toBeGreaterThanOrEqual(1);
  });

  it("shows None labels when no orders", async () => {
    mockFetch({ data: EMPTY });
    render(<App />);
    const items = await screen.findAllByText("None");
    expect(items.length).toBeGreaterThanOrEqual(1);
  });
});
