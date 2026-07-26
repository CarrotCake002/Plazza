#include "Plazza.hpp"
#include "crow.h"
#include <nlohmann/json.hpp>

static int run(float speed_multi, int cook_nb, int restock_timer) {
    Reception reception(speed_multi, cook_nb, restock_timer);
    return reception.run();
}

static int runWithServer(float speed_multi, int cook_nb, int restock_timer, int port) {
    Reception reception(speed_multi, cook_nb, restock_timer);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/status")
    ([&reception]() {
        nlohmann::json j;

        j["config"]["speed_multiplier"] = reception.getSpeedMultiplier();
        j["config"]["cooks_per_kitchen"] = reception.getCookNb();
        j["config"]["restock_timer_ms"] = reception.getRestockTimer();

        auto kitchens = reception.getKitchens();
        auto kitchens_json = nlohmann::json::array();
        for (size_t i = 0; i < kitchens.size(); i++) {
            nlohmann::json k;
            k["id"] = i;
            k["pid"] = kitchens[i]->pid;
            if (kitchens[i]->status) {
                auto* s = kitchens[i]->status;
                k["orders_queued"] = s->orders_queued.load(std::memory_order_relaxed);
                k["orders_cooking"] = s->orders_cooking.load(std::memory_order_relaxed);
                k["cooks_total"] = s->cooks_total.load(std::memory_order_relaxed);
            }
            kitchens_json.push_back(k);
        }
        j["kitchens"] = kitchens_json;

        auto new_json = nlohmann::json::array();
        for (auto& o : reception.getNewOrders()) {
            new_json.push_back({
                {"type", Pizza::typeToString(o.type)},
                {"size", Pizza::sizeToString(o.size)},
                {"amount", o.amount}
            });
        }
        auto pending_json = nlohmann::json::array();
        for (auto& o : reception.getPendingOrders()) {
            pending_json.push_back({
                {"type", Pizza::typeToString(o.type)},
                {"size", Pizza::sizeToString(o.size)},
                {"amount", o.amount}
            });
        }
        j["orders"]["new"] = new_json;
        j["orders"]["pending"] = pending_json;

        return crow::response(j.dump());
    });

    auto server_future = app.bindaddr("127.0.0.1").port(port).multithreaded().run_async();

    int result = reception.run();

    app.stop();
    server_future.wait();
    return result;
}

int main(int ac, char **av) {
    int status = errorHandling(ac, (const char **)av);

    if (status == 1)
        return 0;
    else if (status == -1)
        return 84;

    if (ac == 5)
        return runWithServer(std::stof(av[1]), std::stoi(av[2]), std::stoi(av[3]), std::stoi(av[4]));
    return run(std::stof(av[1]), std::stoi(av[2]), std::stoi(av[3]));
}
