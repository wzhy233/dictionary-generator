#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <algorithm>
#include <limits>
#include <cstring>

#if defined(__GNUC__) || defined(__clang__)
#define OPTIMIZE_O2 __attribute__((optimize("O2")))
#define ALWAYS_INLINE __attribute__((always_inline))
#define HOT __attribute__((hot))
#elif defined(_MSC_VER)
#define OPTIMIZE_O2 __declspec(optimize("2"))
#define ALWAYS_INLINE __forceinline
#define HOT
#else
#define OPTIMIZE_O2
#define ALWAYS_INLINE
#define HOT
#endif

#define int long long

class ObfuscationDictionaryGenerator {
private:
    std::unordered_set<std::string> dictionary;
    std::mutex dict_mutex;
    std::atomic<int> generated_count{0};
    std::atomic<bool> stop_generation{false};
    bool verbose;
    std::chrono::time_point<std::chrono::high_resolution_clock> gen_start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> total_start_time;

    ALWAYS_INLINE HOT OPTIMIZE_O2 int calculateMinLength(int count) {
        if (count <= 0) return 1;
        int length = 1;
        while ((1ULL << length) < static_cast<unsigned long long>(count)) {
            length++;
        }
        return length;
    }

    OPTIMIZE_O2 void generateCombinationsThread(const std::string& initial_prefix, int length, int target_count, int thread_id) {
        if (stop_generation.load(std::memory_order_acquire)) return;

        std::vector<std::string> stack;
        stack.reserve(length * 2);
        stack.push_back(initial_prefix);

        while (!stack.empty() && !stop_generation.load(std::memory_order_acquire)) {
            std::string current = std::move(stack.back());
            stack.pop_back();

            if (current.length() == length) {
                std::lock_guard<std::mutex> lock(dict_mutex);
                if (dictionary.size() < target_count) {
                    auto result = dictionary.insert(std::move(current));
                    if (result.second) {
                        int new_count = ++generated_count;
                        if (verbose && new_count % 10000 == 0) {
                            auto now = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - gen_start_time);
                            std::cout << "[Thread " << thread_id << "] Generated " << new_count
                                      << " strings, elapsed: " << duration.count() << "ms" << std::endl;
                        }
                        if (dictionary.size() >= target_count) {
                            stop_generation.store(true, std::memory_order_release);
                        }
                    }
                }
                continue;
            }

            stack.push_back(current + "I");
            stack.push_back(current + 'l');
        }
    }

    OPTIMIZE_O2 void generateWithThreads(int count, int num_threads) {
        int length = calculateMinLength(count);
        if (verbose) {
            std::cout << "[Info] Minimum length required: " << length << std::endl;
            std::cout << "[Info] Starting generation with " << num_threads << " threads..." << std::endl;
        }

        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        for (int i = 0; i < num_threads; i++) {
            std::string prefix;
            int gray_code = i ^ (i >> 1);
            int prefix_length = (4 < length) ? 4 : length;
            for (int j = 0; j < prefix_length; j++) {
                prefix += (gray_code & (1 << j)) ? 'I' : 'l';
            }

            threads.emplace_back([this, prefix, length, count, i]() {
                if (verbose) {
                    std::cout << "[Info] Thread " << i << " started with prefix: " << prefix << std::endl;
                }
                generateCombinationsThread(prefix, length, count, i);
            });
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

public:
    ObfuscationDictionaryGenerator(bool verbose_mode = false) : verbose(verbose_mode) {}

    OPTIMIZE_O2 void generateDictionary(int count) {
        gen_start_time = std::chrono::high_resolution_clock::now();
        dictionary.clear();
        generated_count = 0;
        stop_generation = false;
        dictionary.reserve(count);

        if (verbose) {
            std::cout << "[Info] Generating " << count << " unique strings..." << std::endl;
        }

        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 8;

        if (count < 100000) {
            num_threads = (num_threads > 4u) ? 4u : num_threads;
        }

        generateWithThreads(count, num_threads);

        auto gen_end_time = std::chrono::high_resolution_clock::now();
        auto gen_duration = std::chrono::duration_cast<std::chrono::milliseconds>(gen_end_time - gen_start_time);

        if (verbose) {
            std::cout << "[Info] Generation completed in " << gen_duration.count() << "ms" << std::endl;
        }
    }

    OPTIMIZE_O2 void printSummary() {
        auto gen_end_time = std::chrono::high_resolution_clock::now();
        auto gen_duration = std::chrono::duration_cast<std::chrono::milliseconds>(gen_end_time - gen_start_time);

        std::cout << "\n=== Generation Summary ===" << std::endl;
        std::cout << "Total strings generated: " << dictionary.size() << std::endl;
        std::cout << "Generation time: " << gen_duration.count() << "ms" << std::endl;
        std::cout << "Performance: " << (dictionary.size() * 1000.0 / gen_duration.count()) << " strings/ms" << std::endl;

        if (!dictionary.empty()) {
            size_t min_len = std::numeric_limits<size_t>::max();
            size_t max_len = 0;
            size_t total_len = 0;

            for (const auto& str : dictionary) {
                size_t len = str.length();
                if (len < min_len) min_len = len;
                if (len > max_len) max_len = len;
                total_len += len;
            }

            std::cout << "Min string length: " << min_len << std::endl;
            std::cout << "Max string length: " << max_len << std::endl;
            std::cout << "Average length: " << (total_len * 1.0 / dictionary.size()) << std::endl;

            std::cout << "Sample strings: ";
            int samples = 0;
            for (const auto& str : dictionary) {
                if (samples >= 5) break;
                std::cout << "\"" << str << "\" ";
                samples++;
            }
            std::cout << std::endl;
        }
    }

    OPTIMIZE_O2 void saveToFile(const std::string& filename) {
        auto save_start = std::chrono::high_resolution_clock::now();

        std::ofstream file(filename, std::ios::binary | std::ios::out);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }

        constexpr size_t buffer_size = 64 * 1024;
        char buffer[buffer_size];
        file.rdbuf()->pubsetbuf(buffer, buffer_size);

        size_t count = 0;
        for (const auto& str : dictionary) {
            file << str << '\n';
            count++;
        }
        file.close();

        auto save_end = std::chrono::high_resolution_clock::now();
        auto save_duration = std::chrono::duration_cast<std::chrono::milliseconds>(save_end - save_start);

        std::cout << "Saved " << count << " strings to " << filename << " in " << save_duration.count() << "ms" << std::endl;
    }

    ALWAYS_INLINE void setVerbose(bool verbose_mode) {
        verbose = verbose_mode;
    }

    ALWAYS_INLINE size_t getDictionarySize() const {
        return dictionary.size();
    }

    ALWAYS_INLINE void clearDictionary() {
        dictionary.clear();
        generated_count = 0;
    }

    ALWAYS_INLINE void startTotalTimer() {
        total_start_time = std::chrono::high_resolution_clock::now();
    }

    ALWAYS_INLINE long long getTotalTime() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - total_start_time).count();
    }
};

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -count <N> [-verbose] [-outfile <filename>]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -count <N>       Number of strings to generate (required)" << std::endl;
    std::cout << "  -verbose         Enable verbose output (optional)" << std::endl;
    std::cout << "  -outfile <file>  Output filename (default: dictionary.txt)" << std::endl;
    std::cout << "  -help            Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " -count 10000 -outfile mydict.txt" << std::endl;
    std::cout << "  " << program_name << " -count 50000 -verbose" << std::endl;
    std::cout << std::endl;
}

OPTIMIZE_O2 signed main(int argc, char* argv[]) {

    if (argc > 1) {
        int count = 0;
        bool verbose = false;
        std::string outfile = "dictionary.txt";
        bool count_provided = false;


        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "/?") == 0) {
                printUsage(argv[0]);
                return 0;
            }
            else if (strcmp(argv[i], "-count") == 0 && i + 1 < argc) {
                count = std::stoll(argv[++i]);
                count_provided = true;
            }
            else if (strcmp(argv[i], "-verbose") == 0) {
                verbose = true;
            }
            else if (strcmp(argv[i], "-outfile") == 0 && i + 1 < argc) {
                outfile = argv[++i];
            }
            else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
                outfile = argv[++i];
            }
            else {
                std::cerr << "Error: Unknown option or missing argument: " << argv[i] << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        }


        if (!count_provided) {
            std::cerr << "Error: -count parameter is required" << std::endl;
            printUsage(argv[0]);
            return 1;
        }

        if (count <= 0) {
            std::cerr << "Error: Count must be greater than 0" << std::endl;
            return 1;
        }

        ObfuscationDictionaryGenerator generator(verbose);

        std::cout << "==========================================" << std::endl;
        std::cout << " Obfuscation Dictionary Generator" << std::endl;
        std::cout << " Made by wzhy233 (support@91net.top)" << std::endl;
        std::cout << "==========================================" << std::endl;

        if (verbose) {
            std::cout << "[System] Available threads: " << std::thread::hardware_concurrency() << std::endl;
        }

        generator.startTotalTimer();
        generator.generateDictionary(count);
        generator.printSummary();
        generator.saveToFile(outfile);

        long long total_time = generator.getTotalTime();
        std::cout << "\n=== Total Execution Time ===" << std::endl;
        std::cout << "Total time: " << total_time << "ms" << std::endl;
        std::cout << "==========================================" << std::endl;

        return 0;
    }

    ObfuscationDictionaryGenerator generator;
    int count;
    char verbose_input;

    std::cout << "==========================================" << std::endl;
    std::cout << " Obfuscation Dictionary Generator" << std::endl;
    std::cout << " Made by wzhy233 (support@91net.top)" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::cout << "Enter number of strings to generate: ";
    std::cin >> count;

    if (count <= 0) {
        std::cerr << "Error: Count must be greater than 0" << std::endl;
        return 1;
    }

    std::cout << "Enable verbose output? (y/n): ";
    std::cin >> verbose_input;
    bool verbose = (verbose_input == 'y' || verbose_input == 'Y');
    generator.setVerbose(verbose);

    generator.startTotalTimer();

    if (verbose) {
        std::cout << "[System] Available threads: " << std::thread::hardware_concurrency() << std::endl;
    }

    generator.generateDictionary(count);
    generator.printSummary();

    std::string filename;
    std::cout << "Enter output filename (default: dictionary.txt): ";
    std::cin.ignore();
    std::getline(std::cin, filename);
    if (filename.empty()) {
        filename = "dictionary.txt";
    }

    generator.saveToFile(filename);

    long long total_time = generator.getTotalTime();
    std::cout << "\n=== Total Execution Time ===" << std::endl;
    std::cout << "Total time: " << total_time << "ms" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}
