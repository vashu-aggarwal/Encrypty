#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include "../encryptDecrypt/Cryption.hpp"
#include <sys/mman.h>
#include <atomic>
#include <fcntl.h>
#include <mutex>
#include <semaphore.h>
#include <thread>

using namespace std;

ProcessManagement::ProcessManagement() {
    itemsSemaphore = sem_open("/items_semaphore", O_CREAT, 0666, 0);
    empty_semaphore = sem_open("/empty_slots_semaphore", O_CREAT, 0666, 1000);
    // if (itemsSemaphore == SEM_FAILED || emptySlotsSemaphore == SEM_FAILED) {
    //     perror("sem_open failed");
    //     exit(EXIT_FAILURE);
    // }
    shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    // if (shmFd == -1) {
    //     perror("shm_open failed");
    //     exit(EXIT_FAILURE);
    // }
    ftruncate(shmFd, sizeof(SharedMemory));
    
    sharedMem = static_cast<SharedMemory *>(mmap(
        nullptr, sizeof(SharedMemory),
        PROT_READ | PROT_WRITE,
        MAP_SHARED, shmFd, 0
    ));

    sharedMem->front = 0;
    sharedMem->rear = 0;
    sharedMem->size.store(0);
}

bool ProcessManagement::submitToQueue(unique_ptr<Task> task) {
    sem_wait(empty_semaphore);
    unique_lock<mutex> lock(queueLock);

    if (sharedMem->size.load() >= 1000) {
        return false;
    }

    strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());
    sharedMem->rear = (sharedMem->rear + 1) % 1000;
    sharedMem->size.fetch_add(1);

    lock.unlock();
    sem_post(itemsSemaphore);

    int pid = fork();
    if (pid < 0) {
        return false;
    } else if (pid > 0) {
        cout << "Entering the parent process" << endl;
    } else {
        cout << "Entering the child process" << endl;
        executeTasks();
        cout << "Exiting the child process" << endl;
        exit(0);
    }
    // std::thread thread_1(&ProcessManagement::executeTasks, this);
    // //detach a thread and run parallely with new thread and only exit when all threads execute completely
    // thread_1.detach();
    return true;
} 

void ProcessManagement::executeTasks() {
    sem_wait(itemsSemaphore);
    unique_lock<mutex> lock(queueLock);

    char taskStr[256];
    strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
    sharedMem->front = (sharedMem->front + 1) % 1000;
    sharedMem->size.fetch_sub(1);

    sem_post(empty_semaphore);
    // cout << "Executing child process" << endl;
    executeCryption(taskStr);
    lock.unlock();
}

ProcessManagement::~ProcessManagement() {
    munmap(sharedMem, sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
}
