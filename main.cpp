#include <iostream>
#include "SimOS.h"

int main() {
    // Create a SimOS instance: 3 disks, 64 GB of RAM, and 1 GB for the OS
    SimOS os(3, 64ULL * 1024 * 1024 * 1024, 1ULL * 1024 * 1024 * 1024); // 64 GB RAM, 1 GB for OS

    // Test: Create a new process with 10 MB size and priority 5
    std::cout << "Creating new process of 10 MB with priority 5...\n";
    if (os.NewProcess(10ULL * 1024 * 1024, 5)) {
        std::cout << "Process created successfully.\n";
    } else {
        std::cout << "Failed to create process.\n";
    }

    // Test: Fork the current process
    std::cout << "Forking the current process...\n";
    if (os.SimFork()) {
        std::cout << "Process forked successfully.\n";
    } else {
        std::cout << "Failed to fork process.\n";
    }

    // Test: Request disk read (disk 0, file "data.txt")
    std::cout << "Requesting disk read for file 'data.txt' on disk 0...\n";
    os.DiskReadRequest(0, "data.txt");

    // Test: Get the process currently using the CPU
    std::cout << "Current process using CPU: PID " << os.GetCPU() << "\n";

    // Test: Get ready-queue
    std::cout << "Ready-queue: ";
    auto readyQueue = os.GetReadyQueue();
    for (int pid : readyQueue) {
        std::cout << pid << " ";
    }
    std::cout << "\n";

    // Test: Check memory usage
    std::cout << "Memory usage:\n";
    auto memoryUsage = os.GetMemory();
    for (const auto& memItem : memoryUsage) {
        std::cout << "PID " << memItem.PID << " uses memory from address "
                  << memItem.itemAddress << " to " << (memItem.itemAddress + memItem.itemSize) << "\n";
    }

    // Test: Simulate a disk job completion
    std::cout << "Simulating disk job completion for disk 0...\n";
    os.DiskJobCompleted(0);

    // Test: Process exits (current process using CPU will exit)
    std::cout << "Exiting the current process...\n";
    os.SimExit();

    // Test: Get the updated ready-queue after exit
    std::cout << "Ready-queue after exit: ";
    readyQueue = os.GetReadyQueue();
    for (int pid : readyQueue) {
        std::cout << pid << " ";
    }
    std::cout << "\n";

    // Test: Create another process to check behavior after exit
    std::cout << "Creating new process of 20 MB with priority 10...\n";
    if (os.NewProcess(20ULL * 1024 * 1024, 10)) {
        std::cout << "Process created successfully.\n";
    } else {
        std::cout << "Failed to create process.\n";
    }

    // Test: Get updated process using CPU
    std::cout << "Current process using CPU after new process creation: PID " << os.GetCPU() << "\n";

    return 0;
}
