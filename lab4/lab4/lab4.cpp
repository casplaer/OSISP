#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <chrono>
#include <random>

const size_t TOTAL_MEMORY_SIZE = 1024 * 1024; 
const size_t BLOCK_SIZE = 1024;               
const size_t NUM_BLOCKS = TOTAL_MEMORY_SIZE / BLOCK_SIZE; 
const size_t NUM_READERS = 5;                 
const size_t NUM_WRITERS = 5;                 
const size_t NUM_OPERATIONS = 20;             

struct MemoryBlock {
    std::vector<char> data;
    std::shared_mutex mtx; 

    MemoryBlock() : data(BLOCK_SIZE, 0) {}
};

std::vector<MemoryBlock> sharedMemory(NUM_BLOCKS);

void readerFunction(int readerId) {
    std::default_random_engine generator(readerId);
    std::uniform_int_distribution<int> blockDistribution(0, NUM_BLOCKS - 1);

    for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
        int blockIndex = blockDistribution(generator);

        {
            std::shared_lock<std::shared_mutex> lock(sharedMemory[blockIndex].mtx);
            char value = sharedMemory[blockIndex].data[0];
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
            std::cout << "Reader " << readerId << " read from block " << blockIndex << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}

void writerFunction(int writerId) {
    std::default_random_engine generator(writerId + 100); 
    std::uniform_int_distribution<int> blockDistribution(0, NUM_BLOCKS - 1);

    for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
        int blockIndex = blockDistribution(generator);

        {
            std::unique_lock<std::shared_mutex> lock(sharedMemory[blockIndex].mtx);
            sharedMemory[blockIndex].data[0] = static_cast<char>(writerId);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
            std::cout << "Writer " << writerId << " wrote to block " << blockIndex << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}

int main() {
    std::vector<std::thread> readers;
    for (int i = 0; i < NUM_READERS; ++i) {
        readers.emplace_back(readerFunction, i);
    }

    std::vector<std::thread> writers;
    for (int i = 0; i < NUM_WRITERS; ++i) {
        writers.emplace_back(writerFunction, i);
    }

    for (auto& reader : readers) {
        reader.join();
    }

    for (auto& writer : writers) {
        writer.join();
    }

    std::cout << "All readers and writers have finished their operations.\n";
    return 0;
}
