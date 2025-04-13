/*
Name: Christopher Zanabria
Date: 4/13/2025
CSCO 340 Spring 25 Term
Professor Shostak
Home Project - OS Simulation
*/

#ifndef SIMOS_H
#define SIMOS_H

#include <vector>
#include <queue>
#include <string>
#include <unordered_map>

struct FileReadRequest {
    int PID{0};
    std::string fileName{""};
};

struct MemoryItem {
    unsigned long long itemAddress;
    unsigned long long itemSize;
    int PID; // PID of the process using this chunk of memory
};

using MemoryUse = std::vector<MemoryItem>;

constexpr int NO_PROCESS{-1};

class SimOS {
public:
    SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned long long sizeOfOS);

    bool NewProcess(unsigned long long size, int priority);
    bool SimFork();
    void SimExit();
    void SimWait();
    void DiskReadRequest(int diskNumber, std::string fileName);
    void DiskJobCompleted(int diskNumber);

    int GetCPU();
    std::vector<int> GetReadyQueue();
    MemoryUse GetMemory();
    FileReadRequest GetDisk(int diskNumber);
    std::queue<FileReadRequest> GetDiskQueue(int diskNumber);

private:
    struct Process {
        int PID;
        int priority;
        unsigned long long memorySize;
        bool isZombie;
        bool isWaitingForChild;
        std::vector<int> children;
        int parentPID;
        bool isForked;
    };

    int nextPID;
    unsigned long long totalRAM;
    unsigned long long osSize;
    int currentCPU;
    std::vector<int> readyQueue;
    std::vector<Process> processes;
    MemoryUse memory;
    std::unordered_map<int, std::queue<FileReadRequest>> diskQueues;

    bool allocateMemory(unsigned long long size, int PID);
    void releaseMemory(int PID);
    void cascadeTermination(int PID);
    int findProcess(int PID);
};

#endif // SIMOS_H
