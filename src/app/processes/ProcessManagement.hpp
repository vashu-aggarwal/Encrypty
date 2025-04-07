#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

#include "Task.hpp"
#include <queue>
#include <memory>
using namespace std;

class ProcessManagement
{
public:
    ProcessManagement(); //just initialises queue-constructor
    bool submitToQueue(unique_ptr<Task> task); //comes from <memory> package & Adds tasks to the queue
    void executeTasks(); //// Executes tasks in the queue

private:   
    // Queue to hold tasks
    //A FIFO (First-In-First-Out) structure for task scheduling.
    //Stores tasks dynamically using unique_ptr, preventing memory leaks.

    //queue<unique_ptr<Task>> taskQueue;
    //(Multiprocessing-->)queue is not gettimg shared among processes, causing infinite loop
    //we will now create a shared memory struct

    struct SharedMemory {
        atomic<int>size;
        char tasks[1000][256];
        int front;
        int rear;

        void printSharedMemory() {
            cout<<size<<endl;
            cout<<front<<endl;
            cout<<rear<<endl;
        }

    };
    SharedMemory* sharedMem;
    int shmFd;
    const char* SHM_NAME = "/my_queue";

};

#endif
/*
unique_ptr<Task> ensures automatic memory management.
When a Task is added to taskQueue, it cannot be shared elsewhere (no accidental memory leaks).
unique_ptr means only one owner for each Task—it will be destroyed when no longer needed.
*/