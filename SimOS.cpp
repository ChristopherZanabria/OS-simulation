#include "SimOS.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>

SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned long long sizeOfOS)
    : nextPID(2), totalRAM(amountOfRAM), osSize(sizeOfOS), currentCPU(1) {
    // OS occupies the initial part of memory
    memory.push_back({0, sizeOfOS, 1});
}

bool SimOS::NewProcess(unsigned long long size, int priority) {
    if (size > totalRAM) return false; // Not enough memory

    // Allocate memory for the new process
    if (!allocateMemory(size, nextPID)) return false;

    Process newProcess = {nextPID++, priority, size, false, false, {}, 0, false};
    processes.push_back(newProcess);

    // Add to the ready-queue if there's no CPU process
    if (currentCPU == NO_PROCESS) {
        currentCPU = newProcess.PID;
    } else {
        readyQueue.push_back(newProcess.PID);
    }

    return true;
}

bool SimOS::SimFork() {
    if (currentCPU == NO_PROCESS || currentCPU == 1) return false;

    Process &parent = processes[findProcess(currentCPU)];
    if (parent.isZombie) return false; // Can't fork if parent is a zombie

    // Try to create the child process with same priority and memory size
    if (!NewProcess(parent.memorySize, parent.priority)) return false;

    // Make this process a parent and set up the child's details
    int childPID = nextPID - 1;
    parent.children.push_back(childPID);
    processes[findProcess(childPID)].parentPID = currentCPU;

    return true;
}

void SimOS::SimExit() {
    if (currentCPU == 1) return; // OS process can't exit

    Process &processToExit = processes[findProcess(currentCPU)];
    processToExit.isZombie = true;

    releaseMemory(currentCPU);

    // Cascade termination if necessary
    cascadeTermination(currentCPU);

    // Check if parent is waiting for child
    if (processToExit.parentPID != 0) {
        Process &parent = processes[findProcess(processToExit.parentPID)];
        if (parent.isWaitingForChild) {
            // Parent resumes execution
            currentCPU = processToExit.parentPID;
            parent.isWaitingForChild = false;
        } else {
            readyQueue.push_back(processToExit.parentPID);
        }
    } else {
        currentCPU = NO_PROCESS; // No process left
    }
}

void SimOS::SimWait() {
    if (currentCPU == 1) return; // OS process can't wait

    Process &parent = processes[findProcess(currentCPU)];
    if (parent.isZombie) return;

    for (int childPID : parent.children) {
        Process &child = processes[findProcess(childPID)];
        if (child.isZombie) {
            parent.isWaitingForChild = false;
            break;
        }
    }
    readyQueue.push_back(currentCPU);
    currentCPU = NO_PROCESS;
}

void SimOS::DiskReadRequest(int diskNumber, std::string fileName) {
    if (currentCPU == 1) return;

    FileReadRequest request = {currentCPU, fileName};
    diskQueues[diskNumber].push(request);
    currentCPU = NO_PROCESS;
}

void SimOS::DiskJobCompleted(int diskNumber) {
    if (diskQueues[diskNumber].empty()) return;

    FileReadRequest completedRequest = diskQueues[diskNumber].front();
    diskQueues[diskNumber].pop();
    
    if (currentCPU == NO_PROCESS) {
        currentCPU = completedRequest.PID;
    } else {
        readyQueue.push_back(completedRequest.PID);
    }
}

int SimOS::GetCPU() {
    return currentCPU;
}

std::vector<int> SimOS::GetReadyQueue() {
    return readyQueue;
}

MemoryUse SimOS::GetMemory() {
    return memory;
}

FileReadRequest SimOS::GetDisk(int diskNumber) {
    if (diskQueues[diskNumber].empty()) {
        return FileReadRequest{0, ""};
    }
    return diskQueues[diskNumber].front();
}

std::queue<FileReadRequest> SimOS::GetDiskQueue(int diskNumber) {
    return diskQueues[diskNumber];
}

bool SimOS::allocateMemory(unsigned long long size, int PID) {
    if (size > totalRAM) return false;

    unsigned long long address = osSize;
    memory.push_back({address, size, PID});
    totalRAM -= size;
    return true;
}

void SimOS::releaseMemory(int PID) {
    auto it = std::remove_if(memory.begin(), memory.end(), [PID](const MemoryItem& item) {
        return item.PID == PID;
    });
    memory.erase(it, memory.end());
}

void SimOS::cascadeTermination(int PID) {
    for (int childPID : processes[findProcess(PID)].children) {
        if (!processes[findProcess(childPID)].isZombie) {
            SimExit(); // Terminate children recursively
        }
    }
}

int SimOS::findProcess(int PID) {
    for (size_t i = 0; i < processes.size(); i++) {
        if (processes[i].PID == PID) return i;
    }
    return -1;
}
