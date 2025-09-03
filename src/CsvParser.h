#pragma once
#include <string>
#include <vector>

// Simple CSV parser with support for quoted fields
inline std::vector<std::string> parse_csv_line(const std::string &line) {
    std::vector<std::string> out;
    std::string field;
    bool in_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (in_quotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i+1] == '"') {
                    field.push_back('"');
                    ++i;
                } else {
                    in_quotes = false;
                }
            } else {
                field.push_back(c);
            }
        } else {
            if (c == '"') {
                in_quotes = true;
            } else if (c == ',') {
                out.push_back(std::move(field));
                field.clear();
            } else {
                field.push_back(c);
            }
        }
    }
    out.push_back(std::move(field));
    return out;
}
