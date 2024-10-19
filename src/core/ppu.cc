#include "ppu.h"
#include <cstring>

#define TOTAL_SCANLINE          261

//	NES color palette. Taken from emudev.de
uint32_t PALETTE[64] = {
	0x7C7C7CFF, 0x0000FCFF, 0x0000BCFF, 0x4428BCFF, 0x940084FF, 0xA80020FF, 0xA81000FF, 0x881400FF,
    0x503000FF, 0x007800FF, 0x006800FF, 0x005800FF, 0x004058FF, 0x000000FF, 0x000000FF, 0x000000FF,
	0xBCBCBCFF, 0x0078F8FF, 0x0058F8FF, 0x6844FCFF, 0xD800CCFF, 0xE40058FF, 0xF83800FF, 0xE45C10FF,
    0xAC7C00FF, 0x00B800FF, 0x00A800FF, 0x00A844FF, 0x008888FF, 0x000000FF, 0x000000FF, 0x000000FF,
    0xF8F8F8FF, 0x3CBCFCFF, 0x6888FCFF, 0x9878F8FF, 0xF878F8FF, 0xF85898FF, 0xF87858FF, 0xFCA044FF,
    0xF8B800FF, 0xB8F818FF, 0x58D854FF, 0x58F898FF, 0x00E8D8FF, 0x787878FF, 0x000000FF, 0x000000FF,
	0xFCFCFCFF, 0xA4E4FCFF, 0xB8B8F8FF, 0xD8B8F8FF, 0xF8B8F8FF, 0xF8A4C0FF, 0xF0D0B0FF, 0xFCE0A8FF,
    0xF8D878FF, 0xD8F878FF, 0xB8F8B8FF, 0xB8F8D8FF, 0x00FCFCFF, 0xF8D8F8FF, 0x000000FF, 0x000000FF,
};

uint16_t PPU::NametableUnmirror(uint16_t addr)
{
    switch (mirrorring) {
    case MIRRORING_HORI:
        if (IN_RANGE(addr, 0x2000, 0x23ff) || IN_RANGE(addr, 0x2400, 0x27ff))
            return (addr & 0x23ff) - 0x2000;
        else if (IN_RANGE(addr, 0x2800, 0x2bff) || IN_RANGE(addr, 0x2c00, 0x2fff))
            return (addr & 0x2bff) - 0x2400;
        break;
    case MIRRORING_VERT:
        if (IN_RANGE(addr, 0x2000, 0x23ff) || IN_RANGE(addr, 0x2400, 0x27ff)) 
            return addr - 0x2000;
        else if (IN_RANGE(addr, 0x2800, 0x2bff) || IN_RANGE(addr, 0x2c00, 0x2fff))
            return addr - 0x2800;
    default:
        break;
    }    
    return 0;
}

uint8_t PPU::MemRead(uint16_t addr)
{
    static uint8_t buf = 0;
    uint8_t ret = 0;

    ret = buf;
    if (IN_RANGE(addr, 0x0000, 0x1fff)) {           // pattern table range
        buf = pROM->chrROMRead(addr & 0x1fff);
    } else if (IN_RANGE(addr, 0x2000, 0x2fff)) {    // nametable range
        buf = VRAM[NametableUnmirror(addr)];
    } else if (IN_RANGE(addr, 0x3f00, 0x3fff)) {    // palette RAM indexes
        addr = addr & 0x3f1f;
        if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
            addr = 0x3f00;
        return paletteRAM[addr - 0x3f00];
    }
    return ret;
}

void PPU::MemWrite(uint16_t addr, uint8_t val)
{
    if (IN_RANGE(addr, 0x0000, 0x1fff)) {           // pattern table range

    } else if (IN_RANGE(addr, 0x2000, 0x2fff)) {    // nametable range
        VRAM[NametableUnmirror(addr)] = val;
    } else if (IN_RANGE(addr, 0x3f00, 0x3fff)) {    // palette RAM indexes
        addr &= 0x3f1f;
        if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
            addr = 0x3f00;
        paletteRAM[addr - 0x3f00] = val;
    }
}

uint8_t PPU::RegRead(uint16_t addr)
{
    switch (addr) {
    case PPU_REG_PPUSTATUS:
        genLatch = (genLatch & 0x1f) | (PPUSTATUS.raw & 0xe0);
        w = 0;
        PPUSTATUS.vblankStarted = 0;
        break;
    case PPU_REG_PPUDATA:
        genLatch = MemRead(v);
        v += PPUCTRL.vramIncrement;
        break;
    case PPU_REG_OAMDATA:
        genLatch = OAM[oamAddr];
        break;
    default:
        break;
    }
    return genLatch;
}

void PPU::RegWrite(uint16_t addr, uint8_t val)
{
    switch (addr) {
    case PPU_REG_PPUCTRL:
        /* t: ...GH.. ........ <- d: ......GH */
        /*    <used elsewhere> <- d: ABCDEF.. */
        t = (t & 0xf3ff) | (((uint16_t)val & 0x03) << 10);

        PPUCTRL.raw = val;
        PPUCTRL.baseNTAddr = 0x2000 + (PPUCTRL.raw & 0x03);
        PPUCTRL.vramIncrement = (NTHBIT(PPUCTRL.raw, 2)) ? 32 : 1;
        PPUCTRL.objPTAddr = (NTHBIT(PPUCTRL.raw, 3)) ? 0x1000 : 0x0000;
        PPUCTRL.bgPTAddr = (NTHBIT(PPUCTRL.raw, 4)) ? 0x1000 : 0x0000;
        PPUCTRL.objHeight = 8 * (1 + (NTHBIT(PPUCTRL.raw, 5)));
        PPUCTRL.vblankNMIEnable = NTHBIT(PPUCTRL.raw, 7);
        break;
    case PPU_REG_PPUMASK:
        PPUMASK.raw = val;
        break;
    case PPU_REG_OAMADDR:
        oamAddr = val;
        break;
    case PPU_REG_OAMDATA:
        OAM[oamAddr++] = val;
        break; 
    case PPU_REG_PPUSCROLL:
        if (!w) {
            t = (t & 0xffe0) | (uint16_t)(val >> 3);
            x = val & 0x07;
            w = 1;
        } else if (w == 1) {
            t = (t & 0x8c1f) | (((uint16_t)val & 0x07) << 12) | (((uint16_t)val >> 3) << 5);
            w = 0;
        }
        break;
    case PPU_REG_PPUADDR:
        if (!w) {
            t = (t & 0xc0ff) | ((uint16_t)(val & 0x3f) << 8);
            t = t & ~(1U << 14);
            w = 1;
        } else if (w == 1) {
            t = (t & 0xff00) | ((uint16_t)val & 0x00ff);
            v = t;
            w = 0;
        }
        break;
    case PPU_REG_PPUDATA:
        MemWrite(v, val);
        v += PPUCTRL.vramIncrement;
        break;
    default:
        break;
    }    
    genLatch = val;
}


uint8_t PPU::ReadPaletteRAM(uint16_t addr)
{
    if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c)
        addr = 0x3f00;
    return paletteRAM[addr - 0x3f00]; 
}

void PPU::UpdatePatternTable()
{
    uint16_t tileAddr = 0;
    uint8_t lsb, msb, pixel;

    // pattern table 0
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            tileAddr = j * 16 + i * 16 * 16;
            for (int k = 0; k < 8; k++) {
                lsb = pROM->chrROMRead(tileAddr + k);
                msb = pROM->chrROMRead(tileAddr + k + 8);
                for (int l = 0; l < 8; l++) {
                    pixel = NTHBIT(lsb, 7 - l) | (NTHBIT(msb, 7 - l) << 1);
                    ptFrameBuffer[l + j * 8 + PATTERN_TABLE_WIDTH * (k + 8 * i)] = PALETTE[ReadPaletteRAM(0x3f00 + pixel)];
                }
            }
        }
    }

    // pattern table 1
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            tileAddr = 0x1000 + j * 16 + i * 16 * 16;
            for (int k = 0; k < 8; k++) {
                lsb = pROM->chrROMRead(tileAddr + k);
                msb = pROM->chrROMRead(tileAddr + k + 8);
                for (int l = 0; l < 8; l++) {
                    pixel = NTHBIT(lsb, 7 - l) | (NTHBIT(msb, 7 - l) << 1);
                    ptFrameBuffer[l + 16 * 8 + j * 8 + PATTERN_TABLE_WIDTH * (k + 8 * i)] = PALETTE[ReadPaletteRAM(0x3f00 + pixel)];
                }
            }
        }
    }
}

void PPU::UpdateTables()
{
    UpdatePatternTable();
}

void PPU::Step(int cycle)
{
    tick += cycle * 3;
    if (tick > 340) {
        tick -= 340;
        if (scanLine == SCREEN_HEIGHT - 1) {
            frameReady = true;
            // and vblank starts
            PPUSTATUS.vblankStarted = true;
        } else if (scanLine == TOTAL_SCANLINE - 1) {
            // vblank ends now
            PPUSTATUS.vblankStarted = false;
        }
        scanLine = (scanLine == TOTAL_SCANLINE - 1) ? 0 : scanLine + 1;
    }
    oldNMI = NMI;
    NMI = (PPUSTATUS.vblankStarted && PPUCTRL.vblankNMIEnable) ? false : true;
}

void PPU::Init(ROM *_ROM, int _mirroring)
{
    mirrorring = (EMirroring)_mirroring;
#if NESTEST
    tick = 21;
#else
    tick = 0;
#endif
    w = 0;
    scanLine = 0;
    genLatch = 0;
    frameReady = false;
    memset(ptFrameBuffer, 0, sizeof(ptFrameBuffer));
    pROM = _ROM;
    oldNMI = NMI = true;
}

bool PPU::IsFrameReady() { return frameReady; }
int PPU::GetScanline() { return scanLine; }
int PPU::GetTicks() { return tick; }
bool PPU::PullNMI() { return (oldNMI && !NMI); }
void PPU::SetFrameNotReady() { frameReady = false; }
uint32_t *PPU::GetPTFrameBuffer() { return ptFrameBuffer; }

/* constructor, destructor */
PPU::PPU() { }
PPU::~PPU() { }