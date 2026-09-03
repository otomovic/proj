//
// Created by os on 8/18/26.
//
#include "../h/Riscv.h"
#include "../lib/hw.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_c.h"
#include "../lib/console.h"
#include "../h/PCB.h"
#include "../h/Semaphore.h"

inline uint64 Riscv::getRegFrame(uint64* trapFrame, uint8 a) {
    return trapFrame[a];
}
inline void Riscv::setRegFrame(uint64* trapFrame, uint8 a, uint64 value) {
    trapFrame[a] = value;
}

void Riscv::handleSupervisorTrap(uint64* trapFrame) {
    uint64 scause;
    asm volatile("csrr %0, scause" : "=r"(scause));

    if (scause == 0x0000000000000009UL || scause == 0x0000000000000008UL) {
        uint64 sepc;
        asm volatile("csrr %0, sepc" : "=r"(sepc));
        sepc += 4;
        asm volatile("csrw sepc, %0" : : "r"(sepc));

        uint64 codeOp = getRegFrame(trapFrame, A0);
        switch (codeOp) {
            case 0x01: {
                uint64 blocks = getRegFrame(trapFrame, A1);
                void* ptr = MemoryAllocator::mem_alloc(blocks * MEM_BLOCK_SIZE);
                setRegFrame(trapFrame, A0, (uint64)ptr);
                return;
            }
            case 0x02: {
                int result = MemoryAllocator::mem_free((void*)getRegFrame(trapFrame, A1));
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x11: {
                thread_t* handle = (thread_t*)getRegFrame(trapFrame, A1);
                PCB::Body startRoutine = (PCB::Body)getRegFrame(trapFrame, A2);
                void* arg = (void*)getRegFrame(trapFrame, A3);
                int result = PCB::create(handle, startRoutine, arg);
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x12: {
                int result = PCB::exit();
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x13: {
                PCB::dispatch();
                return;
            }
            case 0x21: {
                sem_t* handle = (sem_t*)getRegFrame(trapFrame, A1);
                unsigned init = (unsigned)getRegFrame(trapFrame, A2);
                int result = Sem::open(handle, init);
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x22: {
                int result = Sem::close((sem_t)getRegFrame(trapFrame, A1));
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x23: {
                int result = Sem::wait((sem_t)getRegFrame(trapFrame, A1));
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x24: {
                int result = Sem::signal((sem_t)getRegFrame(trapFrame, A1));
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x25: {
                sem_t id = (sem_t)getRegFrame(trapFrame, A1);
                unsigned n = (unsigned)getRegFrame(trapFrame, A2);
                int result = Sem::wait_n(id, n);
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            case 0x26: {
                sem_t id = (sem_t)getRegFrame(trapFrame, A1);
                unsigned n = (unsigned)getRegFrame(trapFrame, A2);
                int result = Sem::signal_n(id, n);
                setRegFrame(trapFrame, A0, (uint64)result);
                return;
            }
            default:
                break;
        }
    }
    else if (scause == 0x8000000000000001UL) {
        // Timer interrupt — not handled (task 4 out of scope).
    }
    else if (scause == 0x8000000000000009) {
        console_handler();
    }
    else {
        // Unexpected trap cause.
    }
}
