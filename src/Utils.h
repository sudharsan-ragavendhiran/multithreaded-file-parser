#pragma once
#include <chrono>
#include <iostream>
#include <string>

struct ScopedTimer {
    using clock = std::chrono::steady_clock;
    clock::time_point start;
    std::string name;

    explicit ScopedTimer(std::string n = "timer")
        : start(clock::now()), name(std::move(n)) {}

    ~ScopedTimer() {
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                       clock::now() - start).count();
        std::cerr << "[TIMER] " << name << " took " << dur << " ms\n";
    }
};
