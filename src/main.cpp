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

    PCB* mainThread;
    PCB::create(&mainThread, [](void*) { userMain(); }, nullptr);
    // Nema još "tekuće" niti — prvi dispatch samo učitava kontekst
    // main-niti i nikad se (u regularnom slučaju) ne vraća ovde, jer
    // se posle svih thread_exit poziva izvršavanje nastavlja unutar
    // poslednje aktivne niti sve dok i ona ne pozove thread_exit.
    PCB::dispatch();

    return 0;
}