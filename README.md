# Multithreaded File Parser

A high-performance, multithreaded file parser for large CSV and NDJSON files.  
Built with modern C++17, using mutexes, condition variables, and a bounded  
producer-consumer queue for thread safety and concurrency.

---

## Features

- Parse CSV or NDJSON files with multiple worker threads  
- Efficient producer-consumer model  
- Thread-safe bounded queue for backpressure  
- Customizable callbacks for record processing  
- Lightweight CSV parsing (no external dependencies)  
- Optional JSON parsing (using [nlohmann/json](https://github.com/nlohmann/json))  

---

## Project Structure

```
multithreaded-file-parser/
├── src/
│   ├── main.cpp                # Entry point (CLI, callbacks, timers)
│   ├── MultithreadedParser.h   # Parser interface
│   ├── MultithreadedParser.cpp # Parser implementation
│   ├── BoundedQueue.h          # Thread-safe queue (header-only)
│   ├── CsvParser.h             # CSV parsing utility
│   └── Utils.h                 # Helpers (ScopedTimer, logging)
```

---

## Build Instructions

Using CMake (recommended):

```bash
git clone https://github.com/sudharsan-ragavendhiran/multithreaded-file-parser.git
cd multithreaded-file-parser
mkdir build
cd build
cmake ..
cmake --build . --parallel
```

This will produce an executable called `parser`.

---

## Usage

```bash
./parser <csv|ndjson> <file_path> [threads] [queue_capacity]
```

- `csv|ndjson` → parsing mode  
- `<file_path>` → path to input file  
- `[threads]` (optional) → number of worker threads (default = CPU cores)  
- `[queue_capacity]` (optional) → queue size for backpressure (default = 16384)  

### Example

```bash
./parser csv data/large_dataset.csv 8 32768
./parser ndjson data/logs.ndjson
```

---

## Example Output

**CSV:**

```
Mode: csv file: data/large_dataset.csv threads: 8 queue_capacity: 32768
[RECORD 0] fields=4 raw=Alice,25,Engineer,NY
[RECORD 1] fields=4 raw=Bob,30,Designer,CA
...
Total CSV records processed: 1000000
[TIMER] CSV parse took 1254 ms
```

**NDJSON:**

```
Mode: ndjson file: data/logs.ndjson threads: 4 queue_capacity: 16384
[NDJSON 0] {"user":"alice","action":"login"}
[NDJSON 1] {"user":"bob","action":"purchase"}
...
Total NDJSON records processed: 500000
[TIMER] NDJSON parse took 987 ms
```

---

## Extending

Replace the callback functions in `main.cpp` to:

- Insert into a database  
- Write to a new file  
- Perform aggregations  

You can also add new parsers (`XmlParser.h`, `ParquetParser.h`, etc.)

---

## License

MIT License © 2025 Sudharsan Ragavendhiran

