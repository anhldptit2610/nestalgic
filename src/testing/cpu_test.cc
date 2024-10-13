#include "cpu_test.h"
#include <yyjson.h>
#include <fstream>

void CPUTest::PrintRegisterState(const std::string& source, int testNum = -1, CPUState *state = nullptr)
{
    printf("-------- %s --------\n", source.c_str());
    if (state != nullptr) {
        printf("Registers - PC: %04x SP: %02x A: %02x X: %02x Y: %02x P: %02x\n", 
            state->PC, state->S, state->A, state->X, state->Y, state->P);
        printf("Flags     - N: %d V: %d 1 B: %d D: %d I: %d Z: %d C: %d\n",
            NTHBIT(state->P, 7), NTHBIT(state->P, 6), NTHBIT(state->P, 4), 
            NTHBIT(state->P, 3), NTHBIT(state->P, 2), NTHBIT(state->P, 1), NTHBIT(state->P, 0));
    } else {
        printf("Registers - PC: %04x SP: %02x A: %02x X: %02x Y: %02x P: %02x\n", 
            regs.PC, regs.S, regs.A, regs.X, regs.Y, flags.P);
        printf("Flags     - N: %d V: %d 1 B: %d D: %d I: %d Z: %d C: %d\n",
            flags.N, flags.V, flags.B, flags.D, flags.I, flags.Z, flags.C);
    }
}

void CPUTest::PrintMemoryState(const std::string& source, int testNum, CPUState *state = nullptr)
{
    printf("-- %s --\n", source.c_str());
    if (state != nullptr) {
        for (auto& i : state->memory)
            printf("%04x ", i.first);
        printf("\n");
        for (auto& i : state->memory)
            printf("%04x ", i.second);
        printf("\n");
    } else {
        for (auto& i : testList[testNum]->final.memory)
            printf("%04x ", i.first);
        printf("\n");
        for (auto& i : testList[testNum]->final.memory)
            printf("%04x ", ReadByte(i.first));
        printf("\n");
    }
}

ETestResult CPUTest::VerifyTest(int testNum)
{
    CPUState& finalState = testList[testNum]->final;

    if (regs.PC != finalState.PC || regs.A != finalState.A || regs.X != finalState.X ||
        regs.Y != finalState.Y || regs.S != finalState.S || flags.P != finalState.P)
        return TR_ERR_REG_DIFF;
    for (auto& i : finalState.memory) {
        if (ReadByte(i.first) != i.second)
            return TR_ERR_MEM_DIFF;
    }
    if (executedCycles != testList[testNum]->cycles)
        return TR_ERR_CYCLE_DIFF;
    return TR_OK;
}

void CPUTest::ShowError(ETestResult result, int testNumber)
{
    printf("Test number: %d\n", testNumber);
    switch (result) {
    case TR_ERR_REG_DIFF:
        printf("Error: Wrong register state.\n");
        PrintRegisterState("initial", testNumber, &testList[testNumber]->initial);
        PrintRegisterState("CPU", testNumber);
        PrintRegisterState("final", testNumber, &testList[testNumber]->final);
        break;
    case TR_ERR_MEM_DIFF:
        printf("Error: Wrong memory state.\n");
        PrintMemoryState("initial", testNumber, &testList[testNumber]->initial);
        PrintMemoryState("CPU", testNumber);
        PrintMemoryState("final", testNumber, &testList[testNumber]->final);
        break;
    case TR_ERR_CYCLE_DIFF:
        printf("Error: Wrong executed cycles.\n");
        printf("CPU: %d - Test: %d\n", executedCycles, testList[testNumber]->cycles);
        break;
    default:
        break;
    }
}

void CPUTest::LoadTest(std::shared_ptr<Test> test)
{
    regs.PC = test->initial.PC;
    regs.S = test->initial.S;
    regs.A = test->initial.A;
    regs.X = test->initial.X;
    regs.Y = test->initial.Y;
    flags.P = test->initial.P;
    for (auto& i : test->initial.memory) {
        WriteByte(i.first, i.second);
    }
}

ETestResult CPUTest::RunTest()
{
    int j = 0;
    try {
        for (auto& i : testList) {
            LoadTest(i); 
            executedCycles = Step();
            ETestResult result = VerifyTest(j);
            if (result != TR_OK)
                throw std::pair<ETestResult, int>(result, j);
            j++;
        }
    } catch (std::pair<ETestResult, int> a) {
        ShowError(a.first, a.second);
        return a.first;
    }
    return TR_OK;
}

uint8_t CPUTest::ReadByte(uint16_t addr) {return RAM[addr]; }
uint16_t CPUTest::ReadWord(uint16_t addr) { return U16(RAM[addr], RAM[addr + 1]); }
void CPUTest::WriteByte(uint16_t addr, uint8_t val) { RAM[addr] = val; }
void CPUTest::WriteWord(uint16_t addr, uint16_t val) { RAM[addr] = LSB(val); RAM[addr + 1] = MSB(val); }

CPUTest::CPUTest(char *testFile) : CPU()
{
    yyjson_read_err err;

    yyjson_doc *doc = yyjson_read_file(testFile, 0, NULL, &err);
    if (doc) {
        yyjson_val *obj = yyjson_doc_get_root(doc), *val = nullptr;
        int idx, max;
        yyjson_arr_foreach(obj, idx, max, val) {
            std::shared_ptr<Test> t = std::make_shared<Test>();
            yyjson_val *ramVal, *PC, *SP, *A, *X, *Y, *P;
            int objIdx, objMax;

            yyjson_val *name = yyjson_obj_get(val, "name");
            t->name = yyjson_get_str(name);

            yyjson_val *initial = yyjson_obj_get(val, "initial");
            PC = yyjson_obj_get(initial, "pc"); t->initial.PC = yyjson_get_int(PC);
            SP = yyjson_obj_get(initial, "s"); t->initial.S = yyjson_get_int(SP);
            A = yyjson_obj_get(initial, "a"); t->initial.A = yyjson_get_int(A);
            X = yyjson_obj_get(initial, "x"); t->initial.X = yyjson_get_int(X);
            Y = yyjson_obj_get(initial, "y"); t->initial.Y = yyjson_get_int(Y);
            P = yyjson_obj_get(initial, "p"); t->initial.P = yyjson_get_int(P);

            yyjson_val *RAM = yyjson_obj_get(initial, "ram");
            yyjson_arr_foreach(RAM, objIdx, objMax, ramVal) {
                std::pair<int, int> memPair;
                yyjson_val *addr = yyjson_arr_get(ramVal, 0);
                yyjson_val *val = yyjson_arr_get(ramVal, 1);

                memPair.first = yyjson_get_int(addr);
                memPair.second = yyjson_get_int(val);
                t->initial.memory.push_back(memPair);
            }

            yyjson_val *final = yyjson_obj_get(val, "final");
            PC = yyjson_obj_get(final, "pc"); t->final.PC = yyjson_get_int(PC);
            SP = yyjson_obj_get(final, "s"); t->final.S = yyjson_get_int(SP);
            A = yyjson_obj_get(final, "a"); t->final.A = yyjson_get_int(A);
            X = yyjson_obj_get(final, "x"); t->final.X = yyjson_get_int(X);
            Y = yyjson_obj_get(final, "y"); t->final.Y = yyjson_get_int(Y);
            P = yyjson_obj_get(final, "p"); t->final.P = yyjson_get_int(P);
            RAM = yyjson_obj_get(final, "ram");
            yyjson_arr_foreach(RAM, objIdx, objMax, ramVal) {
                yyjson_val *memInst = nullptr;
                int ramIdx, ramMax, i = 0, a[2];
                std::pair<int, int> memPair;

                yyjson_arr_foreach(ramVal, ramIdx, ramMax, memInst) {
                    if (i == 2) {
                        memPair.first = a[0];
                        memPair.second = a[1];
                        t->final.memory.push_back(memPair);
                        i = 0;
                    } else {
                        a[i++] = yyjson_get_int(memInst);
                    }
                }
            }
            testList.push_back(t);

            yyjson_val *cycle = yyjson_obj_get(val, "cycles");
            t->cycles = (int)yyjson_arr_size(cycle);
        }
    } else {
        printf("read error\n");
    }
    yyjson_doc_free(doc);

    memset(RAM, 0, TEST_RAM_SIZE);
}

CPUTest::~CPUTest() {}