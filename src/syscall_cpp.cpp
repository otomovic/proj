//
// Created by os on 8/28/26.
//
#include "../h/syscall_cpp.h"

void* operator new(size_t size) { return mem_alloc(size); }
void* operator new[](size_t size) { return mem_alloc(size); }
void operator delete(void* ptr) noexcept { mem_free(ptr); }
void operator delete[](void* ptr) noexcept { mem_free(ptr); }

void Thread::invoke() {
    if (body) body(arg);
    else run();
}
void threadStartRoutine(void* arg) {
    Thread* self = (Thread*)arg;
    self->invoke();
}
Thread::Thread(void (*body_)(void*), void* arg_) : myHandle(nullptr), body(body_), arg(arg_) {}
Thread::Thread() : myHandle(nullptr), body(nullptr), arg(nullptr) {}
Thread::~Thread() {}
int Thread::start() { return thread_create(&myHandle, threadStartRoutine, this); }
void Thread::dispatch() { thread_dispatch(); }
int Thread::sleep(time_t) { return -1; }

Semaphore::Semaphore(unsigned init) : myHandle(nullptr) { sem_open(&myHandle, init); }
Semaphore::~Semaphore() { sem_close(myHandle); }
int Semaphore::wait() { return sem_wait(myHandle); }
int Semaphore::signal() { return sem_signal(myHandle); }

PeriodicThread::PeriodicThread(time_t period_) : Thread(), period(period_) {}
void PeriodicThread::terminate() {}

char Console::getc() { return ::getc(); }
void Console::putc(char c) { ::putc(c); }