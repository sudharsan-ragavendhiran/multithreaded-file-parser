#include "MultithreadedParser.h"
#include "BoundedQueue.h"
#include "CsvParser.h"

#include <atomic>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

struct MultithreadedParser::Impl {
    enum class Mode { CSV, NDJSON };

    BoundedQueue<std::string> queue;
    std::vector<std::thread> workers;
    std::atomic<bool> running{false};
    std::atomic<size_t> processed{0};

    Impl(size_t workers_count, size_t queue_capacity)
        : queue(queue_capacity), workers(workers_count) {}

    template<typename CSV_CB, typename ND_CB>
    bool parse_generic(const std::string &path, Mode mode, CSV_CB csv_cb, ND_CB nd_cb, size_t report_every) {
        if (running) return false;
        running = true;
        processed.store(0);

        // Workers
        for (size_t i = 0; i < workers.size(); ++i) {
            workers[i] = std::thread([&, i, mode, report_every] {
                worker_loop(i, mode, csv_cb, nd_cb, report_every);
            });
        }

        // Producer
        bool ok = read_file(path);
        queue.close();

        for (auto &t : workers) if (t.joinable()) t.join();
        running = false;
        return ok;
    }

    bool read_file(const std::string &path) {
        std::ifstream ifs(path);
        if (!ifs) {
            std::cerr << "Failed to open: " << path << "\n";
            return false;
        }
        std::string line;
        while (std::getline(ifs, line)) {
            if (!queue.push(std::move(line))) break;
            line.clear();
        }
        return true;
    }

    template<typename CSV_CB, typename ND_CB>
    void worker_loop(size_t id, Mode mode, CSV_CB &csv_cb, ND_CB &nd_cb, size_t report_every) {
        std::optional<std::string> maybe;
        size_t local = 0;
        while ((maybe = queue.pop()).has_value()) {
            std::string raw = std::move(maybe.value());
            size_t idx = processed.fetch_add(1);
            if (mode == Mode::CSV) {
                auto fields = parse_csv_line(raw);
                if (csv_cb) csv_cb(idx, fields, raw);
            } else {
                if (nd_cb) nd_cb(idx, raw);
            }
            if (++local % report_every == 0) {
                std::cerr << "[Worker " << id << "] processed " << local << " items\n";
            }
        }
    }
};

// ------------------- Public API -------------------
MultithreadedParser::MultithreadedParser(size_t worker_count, size_t queue_capacity)
    : pimpl_(new Impl(worker_count, queue_capacity)) {}

MultithreadedParser::~MultithreadedParser() {
    delete pimpl_;
}

bool MultithreadedParser::parse_csv_file(const std::string &path, CsvCallback cb, size_t report_every) {
    return pimpl_->parse_generic(path, Impl::Mode::CSV, cb, NdjsonCallback(), report_every);
}

bool MultithreadedParser::parse_ndjson_file(const std::string &path, NdjsonCallback cb, size_t report_every) {
    return pimpl_->parse_generic(path, Impl::Mode::NDJSON, CsvCallback(), cb, report_every);
}
