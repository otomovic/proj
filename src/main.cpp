#include "../lib/mem.h"
#include "../h/main.h"
#include "../h/MemoryAllocator.h"
#include "../lib/console.h"
#include "../h/syscall_c.h"
#include "../h/Riscv.h"
#include "../h/PCB.h"

void userMain();

int main() {
    MemoryAllocator::mem_start();

    __asm__ volatile ("csrw stvec, %0" : : "r"(&Riscv::supervisorTrap));

    userMain();

    return 0;
}