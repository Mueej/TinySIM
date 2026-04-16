/* Copyright 2026 Mueej Al Basit <mueejalbasit01@gmail.com>
 * COA Group Assignment:- Simulating a Simple Cache Controller FSM
 */

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

enum State {
    IDLE,
    COMPARE_TAG,
    ALLOCATE,
    WRITE_BACK
};

struct block {
    bool valid;
    int data;
    int tag;
    bool dirty;
};

int cycleTime;

int memoryStalls() { // artificial stall to simulate memory access time
    int time = 5 + rand() % 2;
    sleep(time * cycleTime);
    return time;
}

class memory {
    int* arr;
public:
    explicit memory(const int size) {
        arr = new int[size];
        for (int i = 0; i < size; i++) {
            arr[i] = 0;
        }
    }

    int read (const int address) {
        std::cout << "    [Memory] Reading from address " << address << std::endl;
        std::cout << "    [Memory] Time taken: " << memoryStalls() << " clock cycles" << std::endl;
        return arr[address];
    }

    void write (const int address, const int data) {
        std::cout << "    [Memory] Writing to address " << address << std::endl;
        std::cout << "    [Memory] Time taken: " << memoryStalls() << " clock cycles" << std::endl;
        arr[address] = data;
    }

    ~memory() {
        delete[] arr;
        std::cout << "deleted memory" << std::endl;
    }
};

class cache {
    block* arr;
    memory* mem;
public:
    explicit cache(const int cache_size, const int mem_size) {
        mem = new memory(mem_size);
        arr = new block[cache_size];
        for (int i = 0; i < cache_size; i++) {
            arr[i].valid = false;
            arr[i].data = 0;
            arr[i].tag = 0;
            arr[i].dirty = false;
        }
    }

    int read(const int address) {
        int index = address % 64;
        int tag = address / 64;
        State state = IDLE;
        bool done = false;
        int result_data = 0;

        while (!done) {
            switch(state) {
                case IDLE:
                    std::cout << "  State: IDLE -> COMPARE_TAG\n";
                    state = COMPARE_TAG;
                    break;

                case COMPARE_TAG:
                    std::cout << "  State: COMPARE_TAG\n";
                    sleep(cycleTime); // cache access
                    if (arr[index].valid && arr[index].tag == tag) {
                        std::cout << "    [Cache Hit]\n";
                        result_data = arr[index].data;
                        done = true; // transitions to IDLE logically
                    } else {
                        std::cout << "    [Cache Miss]\n";
                        if (arr[index].valid && arr[index].dirty) {
                            std::cout << "    [Dirty block found, transitioning to WRITE_BACK]\n";
                            state = WRITE_BACK;
                        } else {
                            std::cout << "    [Transitioning to ALLOCATE]\n";
                            state = ALLOCATE;
                        }
                    }
                    break;

                case WRITE_BACK:
                    std::cout << "  State: WRITE_BACK\n";
                    {
                        int old_address = arr[index].tag * 64 + index;
                        mem->write(old_address, arr[index].data);
                        arr[index].dirty = false;
                        std::cout << "    [Transitioning to ALLOCATE]\n";
                        state = ALLOCATE;
                    }
                    break;

                case ALLOCATE:
                    std::cout << "  State: ALLOCATE\n";
                    {
                        int fetched_data = mem->read(address);
                        arr[index].data = fetched_data;
                        arr[index].tag = tag;
                        arr[index].valid = true;
                        arr[index].dirty = false;
                        std::cout << "    [Transitioning to COMPARE_TAG]\n";
                        state = COMPARE_TAG;
                    }
                    break;
            }
        }
        return result_data;
    }

    void write(const int address, const int data) {
        int index = address % 64;
        int tag = address / 64;
        State state = IDLE;
        bool done = false;

        while (!done) {
            switch(state) {
                case IDLE:
                    std::cout << "  State: IDLE -> COMPARE_TAG\n";
                    state = COMPARE_TAG;
                    break;

                case COMPARE_TAG:
                    std::cout << "  State: COMPARE_TAG\n";
                    sleep(cycleTime); // cache access
                    if (arr[index].valid && arr[index].tag == tag) {
                        std::cout << "    [Cache Hit]\n";
                        arr[index].data = data;
                        arr[index].dirty = true;
                        done = true; // transitions to IDLE
                    } else {
                        std::cout << "    [Cache Miss]\n";
                        if (arr[index].valid && arr[index].dirty) {
                            std::cout << "    [Dirty block found, transitioning to WRITE_BACK]\n";
                            state = WRITE_BACK;
                        } else {
                            std::cout << "    [Transitioning to ALLOCATE]\n";
                            state = ALLOCATE;
                        }
                    }
                    break;

                case WRITE_BACK:
                    std::cout << "  State: WRITE_BACK\n";
                    {
                        int old_address = arr[index].tag * 64 + index;
                        mem->write(old_address, arr[index].data);
                        arr[index].dirty = false;
                        std::cout << "    [Transitioning to ALLOCATE]\n";
                        state = ALLOCATE;
                    }
                    break;

                case ALLOCATE:
                    std::cout << "  State: ALLOCATE\n";
                    {
                        int fetched_data = mem->read(address);
                        arr[index].data = fetched_data;
                        arr[index].tag = tag;
                        arr[index].valid = true;
                        arr[index].dirty = false;
                        std::cout << "    [Transitioning to COMPARE_TAG]\n";
                        state = COMPARE_TAG;
                    }
                    break;
            }
        }
    }

    ~cache() {
        delete[] arr;
        delete mem;
        std::cout << "deleted cache" << std::endl;
    }
};

int main() {
    srand(time(NULL));

    // 64 block cache will be used and memory size will be 256 blocks.
    int CACHE_SIZE = 64;
    int MEM_SIZE = 256;
    cache Cache(CACHE_SIZE, MEM_SIZE);

    int loopCount, cnt = 0;
    std::cout << "Enter loop count: ";
    std::cin >> loopCount;

    std::cout << "Enter time for 1 clock cycle (seconds): ";
    std::cin >> cycleTime;

    while (cnt++ < loopCount) {
        int req_type = rand() % 2; // read = 1, write = 0
        int mem_add = rand() % 256; // total 256 memory addresses

        if (req_type == 1) { // read
            std::cout << "\n" << cnt << " =================== READ ===================\n";
            std::cout << "Requesting to read address " << mem_add << "\n";

            int data = Cache.read(mem_add);
            std::cout << "Value at address " << mem_add << " is " << data << std::endl;
        } else { // write
            std::cout << "\n" << cnt << " =================== WRITE ==================\n";
            int newData = rand() % 10000;
            std::cout << "Requesting to write value " << newData << " to address " << mem_add << "\n";

            Cache.write(mem_add, newData);
            std::cout << "Updated value at address " << mem_add << " to " << newData << std::endl;
        }
    }

    return 0;
}