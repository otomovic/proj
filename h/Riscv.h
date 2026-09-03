//
// Created by os on 8/18/26.
//
#ifndef PROJECT_BASE_V1_1_TRAP_H
#define PROJECT_BASE_V1_1_TRAP_H
#include "../lib/hw.h"
class Riscv {
public:
    static void handleSupervisorTrap(uint64* trapFrame);
    static uint64 getRegFrame(uint64* trapFrame, uint8 a);
    static void setRegFrame(uint64* trapFrame, uint8 a, uint64 value);
    static void supervisorTrap();
private:
    enum Registers : uint8 {
        ZERO = 0, RA, SP, GP, TP, T0, T1, T2, S0, S1,
        A0, A1, A2, A3, A4, A5, A6, A7,
        S2, S3, S4, S5, S6, S7, S8, S9, S10, S11,
        T3, T4, T5, T6
    };
};
#endif //PROJECT_BASE_V1_1_TRAP_H