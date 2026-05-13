#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <cmath>
#include <chrono>
#include <cstdint>

class DictionaryGenerator {
private:
    uint64_t count;
    std::string output_file;
    bool verbose;

    static uint32_t requiredLength(uint64_t count) {
        if (count <= 1)
            return 1;

        return static_cast<uint32_t>(
            std::ceil(std::log2(static_cast<long double>(count)))
        );
    }

    static std::string encodeBinary(uint64_t value, uint32_t length) {
        std::string result(length, 'l');

        for (uint32_t i = 0; i < length; ++i) {
            if (value & (1ULL << (length - i - 1))) {
                result[i] = 'I';
            }
        }

        return result;
    }

public:
    DictionaryGenerator(
        uint64_t count_,
        std::string outfile_,
        bool verbose_
    )
        : count(count_),
          output_file(std::move(outfile_)),
          verbose(verbose_) {}

    void generate() {
        auto start = std::chrono::high_resolution_clock::now();

        const uint32_t length = requiredLength(count);

        if (verbose) {
            std::cout << "[Info] Required string length: "
                      << length << '\n';
        }

        std::ofstream file(output_file, std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open output file");
        }

        constexpr size_t BUFFER_SIZE = 1 << 20;
        char buffer[BUFFER_SIZE];

        file.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);

        for (uint64_t i = 0; i < count; ++i) {
            file << encodeBinary(i, length) << '\n';

            if (verbose && i % 100000 == 0 && i != 0) {
                std::cout << "[Progress] "
                          << i
                          << " generated\n";
            }
        }

        file.close();

        auto end = std::chrono::high_resolution_clock::now();

        auto ms = std::chrono::duration_cast<
            std::chrono::milliseconds
        >(end - start).count();

        std::cout << "\n=== Generation Complete ===\n";
        std::cout << "Generated : " << count << '\n';
        std::cout << "Length    : " << length << '\n';
        std::cout << "Output    : " << output_file << '\n';
        std::cout << "Time      : " << ms << " ms\n";

        if (ms > 0) {
            std::cout << "Speed     : "
                      << (count * 1000ULL / ms)
                      << " strings/sec\n";
        }
    }
};

static void printUsage(const char* name) {
    std::cout
        << "Usage:\n"
        << "  " << name
        << " -count <number> [-outfile file] [-verbose]\n";
}

int main(int argc, char* argv[]) {
    uint64_t count = 0;
    std::string outfile = "dictionary.txt";
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-count" && i + 1 < argc) {
            count = std::stoull(argv[++i]);
        }
        else if (arg == "-outfile" && i + 1 < argc) {
            outfile = argv[++i];
        }
        else if (arg == "-verbose") {
            verbose = true;
        }
        else if (arg == "-help") {
            printUsage(argv[0]);
            return 0;
        }
        else {
            std::cerr << "Unknown argument: "
                      << arg << '\n';

            return 1;
        }
    }

    if (count == 0) {
        std::cerr << "Count must be > 0\n";
        return 1;
    }

    try {
        DictionaryGenerator generator(
            count,
            outfile,
            verbose
        );

        generator.generate();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: "
                  << ex.what()
                  << '\n';

        return 1;
    }

    return 0;
}
