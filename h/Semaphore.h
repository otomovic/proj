#ifndef PROJECT_BASE_V1_1_SEM_H
#define PROJECT_BASE_V1_1_SEM_H
#include "../lib/hw.h"
#include "PCB.h"

class Sem {
public:
    static int open(Sem** handle, unsigned init);
    static int close(Sem* sem);
    static int wait(Sem* sem);
    static int signal(Sem* sem);
    static int wait_n(Sem* sem, unsigned n);
    static int signal_n(Sem* sem, unsigned n);

private:
    Sem(unsigned init);
    ~Sem() = default;

    unsigned value;
    bool closed;
    PCB* waitHead;
    PCB* waitTail;

    void enqueue(PCB* t, unsigned n);
};
typedef Sem* sem_t;
#endif