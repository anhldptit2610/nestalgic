#ifndef PPU_H_
#define PPU_H_

#include "common.h"
#include "rom.h"

#define PPU_REG_PPUCTRL                     0x2000      /* Access: write */
#define PPU_REG_PPUMASK                     0x2001      /* Access: write */
#define PPU_REG_PPUSTATUS                   0x2002      /* Access: read */
#define PPU_REG_OAMADDR                     0x2003      /* Access: write */
#define PPU_REG_OAMDATA                     0x2004      /* Access: read/write */
#define PPU_REG_PPUSCROLL                   0x2005      /* Access: write x2 */
#define PPU_REG_PPUADDR                     0x2006      /* Access: write x2 */
#define PPU_REG_PPUDATA                     0x2007      /* Access: read/write */
#define PPU_REG_OAMDMA                      0x4014

#define OAM_SIZE                            256


typedef enum {
    MIRRORING_HORI,
    MIRRORING_VERT,
} EMirroring;

class PPU {
private:
    uint8_t genLatch;
    uint8_t VRAM[PPU_RAM_SIZE];
    uint8_t paletteRAM[PPU_PAL_SIZE];
    ROM *pROM = nullptr;
    uint32_t ptFrameBuffer[PATTERN_TABLE_HEIGHT * PATTERN_TABLE_WIDTH];

    struct PPUCTRL {        /* Read/Write */
        uint8_t raw;
        uint16_t baseNTAddr;
        uint8_t vramIncrement;
        uint16_t objPTAddr;
        uint16_t bgPTAddr;
        uint8_t objHeight;
        bool vblankNMIEnable;
    } PPUCTRL;
    union PPUMASK {         /* Read/Write */
        uint8_t raw;
        struct {
            uint8_t greyScaleEnable : 1;
            uint8_t showBGInLeftMostTile : 1;
            uint8_t showOBJInLeftMostTile : 1;
            uint8_t bgRenderingEnable : 1;
            uint8_t objRenderingEnable : 1;
            uint8_t emphasizeRed : 1;
            uint8_t emphasizeGreen : 1;
            uint8_t emphasizeBlue : 1;
        };
    } PPUMASK;
    union PPUSTATUS {       /* Read */
        uint8_t raw;
        struct {
            uint8_t unused : 5;
            uint8_t objOverflow : 1;
            uint8_t obj0Hit : 1;
            uint8_t vblankStarted : 1;
        };
    } PPUSTATUS;
    uint16_t VRAMADDR : 15;

    EMirroring mirrorring;

    /* internal registers a.k.a loopy */
    uint16_t v : 15;
    uint16_t t : 15;
    uint8_t x : 3;
    bool w;

    int tick;
    int scanLine;
    bool frameReady;
    /* NMI */
    bool oldNMI;
    bool NMI;

    /* OAM */
    uint8_t OAM[OAM_SIZE];
    uint16_t oamAddr;

    uint8_t MemRead(uint16_t);
    void MemWrite(uint16_t, uint8_t);
    uint16_t NametableUnmirror(uint16_t);
    uint8_t ReadPaletteRAM(uint16_t);
    void UpdatePatternTable();

    // TODO: scrolling
    // TODO: oamdma
public:
    /* getter/setter */
    int GetScanline();
    int GetTicks();
    bool PullNMI();

    void SetFrameNotReady();
    void UpdateTables();
    uint32_t *GetPTFrameBuffer();
    bool IsFrameReady();    
    uint8_t RegRead(uint16_t);
    void RegWrite(uint16_t, uint8_t);
    void Init(ROM *, int);
    void Step(int);
    PPU();
    ~PPU();
};


#endif