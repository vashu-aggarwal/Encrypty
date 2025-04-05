#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include "../encryptDecrypt/Cryption.hpp"
using namespace std;

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(unique_ptr<Task> task) {
    taskQueue.push(move(task));
    return true;
}

void ProcessManagement::executeTasks() {
    while (!taskQueue.empty()) {
        
        unique_ptr<Task> taskToExecute = move(taskQueue.front());
        taskQueue.pop();
        cout << "Executing task: " << taskToExecute->toString() << endl;
        executeCryption(taskToExecute->toString()); //taskToExecute is a pointer
        
    }
}