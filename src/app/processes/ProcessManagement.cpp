#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include "../encryptDecrypt/Cryption.hpp"
using namespace std;

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(unique_ptr<Task> task) {
    taskQueue.push(move(task));
    int pid = fork();
    if (pid < 0) {
        return false;
    }
    else if (pid>0) {
        cout<<"Entering the parent process"<<endl;
    }
    else{
        cout<<"Entering the child process"<<endl;
        executeTasks();//child process is spinned
        cout<<"Exiting the child process"<<endl;
    }
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