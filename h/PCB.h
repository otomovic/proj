//
// Created by os on 8/28/26.
//
#ifndef PROJECT_BASE_V1_1_PCB_H
#define PROJECT_BASE_V1_1_PCB_H
#include "../lib/hw.h"
class PCB {
public:
    struct Context {
        uint64 ra;
        uint64 sp;
    };
    using Body = void (*)(void*);
    static int create(PCB** handle, Body body, void* arg);
    static int exit();
    static void dispatch();
    static void block();          // park without requeueing (for semaphores)
    static PCB* running;
    static void contextSwitch(Context* oldContext, Context* newContext);
    int wakeupResult;             // 0 = normal wakeup, <0 = e.g. semaphore closed
    unsigned waitN;                // how many units this thread is waiting for (wait_n)
    Context context;
    uint64* stack;
    PCB* next;
private:
    friend class Scheduler;
    friend class Semaphore;
    static void threadWrapper();
    Body body;
    void* arg;
    bool finished;
    PCB(Body body, void* arg, uint64* stack, uint64* context);
};
typedef PCB* thread_t;
#endif //PROJECT_BASE_V1_1_PCB_H