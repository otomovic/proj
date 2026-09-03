//
// Created by os on 8/18/26.
//
#ifndef PROJECT_BASE_V1_1_SYSCALL_C_H
#define PROJECT_BASE_V1_1_SYSCALL_C_H
#include "../lib/hw.h"

class PCB;
typedef PCB* thread_t;
class Sem;
typedef Sem* sem_t;

void* mem_alloc(size_t size);
int mem_free(void* ptr);

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();

int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
int sem_wait_n(sem_t id, unsigned n);
int sem_signal_n(sem_t id, unsigned n);
typedef unsigned long time_t;
int time_sleep (time_t);


const int EOF = -1;
char getc();
void putc(char c);

#endif //PROJECT_BASE_V1_1_SYSCALL_C_H