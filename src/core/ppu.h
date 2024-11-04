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
    DFS_GET_NAMETABLE_BYTE,
    DFS_GET_ATTRIBUTE_BYTE,
    DFS_GET_PT_TILE_LOW,
    DFS_GET_PT_TILE_HIGH,
} EDataFetchStep;

typedef struct Pos {
    uint8_t raw;
    uint8_t Coarse() const { return raw / 8; }
    uint8_t Fine() const { return raw % 8; }
    uint8_t Add8() const { return raw + 8; }
    uint8_t NextCoarse() const { return (raw + 8) / 8; }
    Pos& operator=(const uint8_t r) {
        this->raw = r;
        return *this;
    }
    void operator++() {
        this->raw = (this->raw + 1) % SCREEN_WIDTH;
    }
} Pos;


typedef struct {
    uint8_t yPos;
    uint8_t tileIndex;
    union {
        uint8_t raw;
        struct {
            uint8_t palette : 2;
            uint8_t unused : 3;
            uint8_t priority : 1;
            uint8_t flipOBJHorizontally : 1;
            uint8_t flipOBJVertically : 1;
        };
    } attribute;
    uint8_t xPos;
} OAMEntry;

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
    uint32_t objFrameBuffer[OBJ_TABLE_HEIGHT * OBJ_TABLE_WIDTH];
    uint32_t screenFrameBuffer[SCREEN_HEIGHT * SCREEN_WIDTH];
    uint32_t tileMapFrameBuffer[TILE_MAP_WIDTH * TILE_MAP_HEIGHT];

    union {
        uint8_t raw;
        struct {
            uint8_t baseNTAddr : 2;
            uint8_t vramIncrement : 1;
            uint8_t objPTAddr : 1;
            uint8_t bgPTAddr : 1;
            uint8_t objHeight : 1;
            uint8_t ppuMasterSelect : 1;
            uint8_t vblankNMIEnable : 1;
        };
    } PPUCTRL;              /* Read/Write */

    union {
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
    } PPUMASK;              /* Read/Write */

    union {
        uint8_t raw;
        struct {
            uint8_t unused : 5;
            uint8_t objOverflow : 1;
            uint8_t obj0Hit : 1;
            uint8_t vblankStarted : 1;
        };
    } PPUSTATUS;            /* Read */

    EMirroring mirrorring;

    /* internal registers a.k.a loopy */
    uint16_t loopyV : 15;           /* current VRAM address (15 bits) */
    uint16_t loopyT : 15;           /* temporary VRAM address (15 bits) */
    uint8_t loopyX : 3;             /* fine X scroll (3 bits) */
    uint8_t loopyW : 1;             /* first or second write toggle (1 bit) */

    int tick;
    int scanLine;
    bool frameReady;

    /* rendering internal */
    Pos xPos;
    int currentXPos;
    uint8_t nextPTDataLow;
    uint8_t nextPTDataHigh;
    uint8_t nextAttribute;
    uint8_t currentPTDataLow;
    uint8_t currentPTDataHigh;
    uint8_t currentAttribute;

    /* NMI */
    bool oldNMI;
    bool NMI;

    /* OAM */
    uint8_t OAM[OAM_SIZE];
    uint16_t oamAddr;

    uint8_t MemRead(uint16_t) const;
    void MemWrite(uint16_t, uint8_t);
    uint16_t NametableUnmirror(uint16_t) const;
    uint8_t ReadPaletteRAM(uint16_t) const;
    void UpdatePatternTable();
    void UpdateObjTable();
    void UpdateTileMap();
    uint8_t MemReadNoBuf(uint16_t) const;
    uint8_t GetAttributeTableEntry(unsigned, int, int) const;
    void UpdateLoopyV();
    bool RenderEnable() const;
    bool DuringRendering() const;
    void DrawBgTile(uint16_t, uint8_t, uint8_t);

public:
    /* getter/setter */
    int GetScanline() const;
    int GetTicks() const;
    bool PullNMI() const;
    uint32_t *GetPTFrameBuffer();
    uint32_t *GetScreenFrameBuffer();
    uint8_t * GetOAM();
    uint32_t *GetOBJFrameBuffer();
    uint32_t *GetTileMapFrameBuffer();
    void GetTileData(int);

    void SetFrameNotReady();
    void UpdateTables();
    bool IsFrameReady() const;
    uint8_t RegRead(uint16_t);
    void RegWrite(uint16_t, uint8_t);
    void Init(ROM *, int);
    void Step(int);

    /* constructor, destructor */
    PPU();
    ~PPU();
};


#endif