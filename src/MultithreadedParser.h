#pragma once
#include <functional>
#include <string>
#include <vector>

class MultithreadedParser {
public:
    using CsvCallback = std::function<void(size_t, const std::vector<std::string>&, const std::string&)>;
    using NdjsonCallback = std::function<void(size_t, const std::string&)>;

    MultithreadedParser(size_t worker_count, size_t queue_capacity);
    ~MultithreadedParser();

    bool parse_csv_file(const std::string &path, CsvCallback cb, size_t report_every = 1'000'000);
    bool parse_ndjson_file(const std::string &path, NdjsonCallback cb, size_t report_every = 1'000'000);

private:
    struct Impl;
    Impl* pimpl_; // PIMPL idiom keeps header clean
};
