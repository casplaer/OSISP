#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <thread>
#include <algorithm>
#include <chrono>

void generate_data_file(const std::string& filename, size_t num_elements) {
    std::ofstream out(filename);
    for (size_t i = 0; i < num_elements; ++i) {
        out << rand() % 100000 << "\n";
    }
}

void traditional_processing(const std::string& input_file, const std::string& output_file) {
    std::ifstream in(input_file);
    std::ofstream out(output_file);
    std::vector<int> data;

    int value;
    while (in >> value) {
        data.push_back(value);
    }

   std::sort(data.begin(), data.end());

    for (const auto& v : data) {
        out << v << "\n";
    }
}

std::vector<int> async_read_data(const std::string& input_file) {
    std::ifstream in(input_file);
    std::vector<int> data;
    int value;
    while (in >> value) {
        data.push_back(value);
    }
    return data;
}

void async_processing(const std::string& input_file, const std::string& output_file) {
    auto read_future = std::async(std::launch::async, async_read_data, input_file);

    auto data = read_future.get();
    auto sort_future = std::async(std::launch::async, [&data]() {
        std::sort(data.begin(), data.end());
        });

    sort_future.wait();

    std::ofstream out(output_file);
    for (const auto& v : data) {
        out << v << "\n";
    }
}

void threaded_processing(const std::string& input_file, const std::string& output_file) {
    std::vector<int> data;

    std::thread read_thread([&]() {
        std::ifstream in(input_file);
        int value;
        while (in >> value) {
            data.push_back(value);
        }
        });

    read_thread.join();

    std::thread sort_thread([&]() {
        std::sort(data.begin(), data.end());
        });

    sort_thread.join();

    std::ofstream out(output_file);
    for (const auto& v : data) {
        out << v << "\n";
    }
}

void measure_performance() {
    setlocale(LC_ALL, "ru");
    const std::string input_file = "data.txt";
    const std::string output_file_trad = "output_trad.txt";
    const std::string output_file_async = "output_async.txt";
    const std::string output_file_thread = "output_thread.txt";
    const size_t num_elements = 1000000;

    generate_data_file(input_file, num_elements);

    auto start = std::chrono::high_resolution_clock::now();
    traditional_processing(input_file, output_file_trad);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Традиционный подход занял: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    async_processing(input_file, output_file_async);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Асинхронный подход занял: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    threaded_processing(input_file, output_file_thread);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Многопоточный подход занял: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
}

int main() {
    measure_performance();
    return 0;
}
