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
    queue<unique_ptr<Task>> taskQueue;  // Queue to hold tasks
    //A FIFO (First-In-First-Out) structure for task scheduling.
    //Stores tasks dynamically using unique_ptr, preventing memory leaks.
};

#endif
/*
unique_ptr<Task> ensures automatic memory management.
When a Task is added to taskQueue, it cannot be shared elsewhere (no accidental memory leaks).
unique_ptr means only one owner for each Task—it will be destroyed when no longer needed.
*/