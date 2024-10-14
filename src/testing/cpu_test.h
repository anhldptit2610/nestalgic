#include "cpu/cpu.h"
#include <fstream>
#include <vector>
#include <memory>

#define TEST_RAM_SIZE           (64 * KiB)

typedef enum {
    TR_OK,
    TR_ERR_REG_DIFF,
    TR_ERR_MEM_DIFF,
    TR_ERR_CYCLE_DIFF,
} ETestResult;

typedef struct {
    uint16_t PC;
    uint8_t S;
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint8_t P;
    std::vector<std::pair<int, int>> memory;
} CPUState;

typedef struct {
    std::string name;
    CPUState initial;
    CPUState final;
    int cycles;
} Test;

class CPUTest : public CPU {
private:
    uint8_t RAM[TEST_RAM_SIZE];
    int executedCycles;
    std::vector<std::shared_ptr<Test>> testList;
    void LoadTest(std::shared_ptr<Test>);
    ETestResult VerifyTest(int);
    void ShowError(ETestResult, int);
    void PrintRegisterState(const std::string&, int, CPUState *);
    void PrintMemoryState(const std::string&, int, CPUState *);

    uint8_t ReadByte(uint16_t addr) override;
    uint16_t ReadWord(uint16_t addr) override;
    void WriteByte(uint16_t addr, uint8_t val) override;
public:
    ETestResult RunTest();
    /* constructor/destructor */
    CPUTest(char *);
    ~CPUTest();
};