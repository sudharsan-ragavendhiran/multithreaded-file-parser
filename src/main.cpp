#include "MultithreadedParser.h"
#include "Utils.h"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <thread>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: parser <csv|ndjson> <file_path> [threads] [queue_capacity]\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string path = argv[2];
    size_t threads = (argc > 3) ? std::stoul(argv[3]) : std::thread::hardware_concurrency();
    size_t queue_capacity = (argc > 4) ? std::stoul(argv[4]) : 16384;

    if (!std::filesystem::exists(path)) {
        std::cerr << "File not found: " << path << "\n";
        return 1;
    }

    MultithreadedParser parser(threads, queue_capacity);
    std::atomic<size_t> counter{0};
    const size_t PRINT_FIRST = 5;

    if (mode == "csv") {
        // Define CSV callback
        MultithreadedParser::CsvCallback csv_cb =
            [&counter](size_t idx, const std::vector<std::string>& fields, const std::string& raw) {
                if (++counter <= 5) {
                    std::cerr << "[CSV " << idx << "] fields=" << fields.size()
                              << " raw=" << raw << "\n";
                }
            };

        ScopedTimer t("CSV parse");
        parser.parse_csv_file(path, csv_cb);
        std::cerr << "Total CSV records processed: " << counter.load() << "\n";
    }
    else if (mode == "ndjson") {
        // Define NDJSON callback
        MultithreadedParser::NdjsonCallback nd_cb =
            [&counter](size_t idx, const std::string& raw) {
                if (++counter <= 5) {
                    std::cerr << "[NDJSON " << idx << "] " << raw << "\n";
                }
            };

        ScopedTimer t("NDJSON parse");
        parser.parse_ndjson_file(path, nd_cb);
        std::cerr << "Total NDJSON records processed: " << counter.load() << "\n";
    }
    else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    return 0;
}
