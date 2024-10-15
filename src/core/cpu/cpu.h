#ifndef CPU_H_
#define CPU_H_

#include "../common.h"
#include "../mmu.h"
#include <string>

#define RAM_SIZE                    2 * KiB
#define STACK_START                 0x0100
#define STACK_END                   0x01ff
#define IRQ_START                   0xfffe
#define IRQ_END                     0xffff

class CPU;

typedef enum ADDRESSING_MODE {
    AM_IMPL,
    AM_ACC,
    AM_IMM,
    AM_ZP,
    AM_ABS,
    AM_REL,
    AM_IND,
    AM_ZPX,
    AM_ZPY,
    AM_ABSX,
    AM_ABSY,
    AM_XIND,
    AM_INDY,
    AM_ILL,
} EAddrMode;

union CPUFlags {
    uint8_t P;
    struct {
        uint8_t C : 1;
        uint8_t Z : 1;
        uint8_t I : 1;
        uint8_t D : 1;
        uint8_t B : 1;
        uint8_t U : 1;
        uint8_t V : 1;
        uint8_t N : 1;
    };
};

struct CPURegisters {
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint16_t PC;
    uint8_t S;
};

typedef struct {
    std::string name;
    EAddrMode addrMode;
    void (CPU::*handler)();
    int baseCycle; 
} Opcode; 

typedef struct {
    uint8_t opcode;
    uint8_t operand[2];
    uint16_t effectiveAddress;
    EAddrMode addressingMode;
    bool pageCrossed;
    int bonusCycles;
} Instruction;

class CPU {
protected:
    CPURegisters regs;
    CPUFlags flags;
private:
    static Opcode opcodeList[256];
    Instruction instr;
    MMU *pMMU;

    /* Load/Store Opcodes */
    void LDA();
    void LDX();
    void LDY();
    void STA();
    void STX();
    void STY();

    /* Register Transfers */
    void TAX();
    void TAY();
    void TXA();
    void TYA();

    /* Stack Operations */
    void TSX();
    void TXS();
    void PHP();
    void PLA();
    void PLP();

    /* Logical Opcodes */
    void AND();
    void EOR();
    void ORA();
    void BIT();

    /* Arithmetic Opcodes */
    void ADC();
    void SBC();
    void CMP();
    void CPX();
    void CPY();

    /* Increments & Decrements */
    void INC();
    void INX();
    void INY();
    void DEC();
    void DEX();
    void DEY();

    /* Shift Opcodes */
    void ASL();
    void LSR();
    void ROL();
    void ROR();

    /* Jumps & Calls */
    void JMP();
    void JSR();
    void RTS();

    /* Branches Opcodes */
    void BCC();
    void BCS();
    void BEQ();
    void BMI();
    void BNE();
    void BPL();
    void BVC();
    void BVS();

    /* Status Flag Changes */
    void CLC();
    void CLD();
    void CLI();
    void CLV();
    void SEC();
    void SED();
    void SEI();

    /* System Functions */
    void BRK();
    void NOP();
    void RTI();

    /* Illegal Opcodes */
    void TAS();
    void JAM();
    void SLO();
    void ANC();
    void RLA();
    void SRE();
    void PHA();
    void ALR();
    void RRA();
    void ARR();
    void SAX();
    void ANE();
    void SHA();
    void SHY();
    void SHX();
    void LAX();
    void LXA();
    void LAS();
    void DCP();
    void SBX();
    void ISC();
    void USBC();

    void GetInstruction();
    void SetNZFlag(uint8_t);
    void SetZNVFlag(uint8_t);
    void SetZNCFlag(uint8_t, bool);
    void HandleOpcode();

    /* bus access methods */
    virtual uint8_t ReadByte(uint16_t);
    virtual uint16_t ReadWord(uint16_t);
    virtual void WriteByte(uint16_t, uint8_t);

    /* stacks push/pop */
    void PushByte(uint8_t);
    void PushWord(uint16_t);
    uint8_t PopByte();
    uint16_t PopWord();
public:
    void WriteRAM(uint16_t, uint8_t);
    uint8_t ReadRAM(uint16_t);
    int Step();
    void Init(MMU *);

    /* constructor/destructor */
    CPU();
    ~CPU();
};

#endif