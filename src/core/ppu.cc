#include "ppu.h"
#include <cstring>

#define TOTAL_SCANLINE          262

//	NES color palette. Taken from emudeloopyV.de
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

uint16_t PPU::NametableUnmirror(uint16_t addr) const
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

uint8_t PPU::GetAttributeTableEntry(unsigned nametable, int x, int y) const
{
    uint16_t atBaseAddr = 0x23c0 + 0x0400 * nametable;
    x /= 4;
    y /= 4;
    return MemReadNoBuf(atBaseAddr + y * 8 + x);
}

uint8_t PPU::MemReadNoBuf(uint16_t addr) const
{
    if (IN_RANGE(addr, 0x0000, 0x1fff))         // pattern table range
        return pROM->chrROMRead(addr & 0x1fff);
    else if (IN_RANGE(addr, 0x2000, 0x2fff))    // nametable range
        return VRAM[NametableUnmirror(addr)];
    else if (IN_RANGE(addr, 0x3f00, 0x3fff))    // palette RAM indexes
        return ReadPaletteRAM(addr);
    return 0xff;
}

uint8_t PPU::MemRead(uint16_t addr) const
{
    static uint8_t buf = 0;
    uint8_t ret = 0;

    ret = buf;
    if (IN_RANGE(addr, 0x0000, 0x1fff))         // pattern table range
        buf = pROM->chrROMRead(addr & 0x1fff);
    else if (IN_RANGE(addr, 0x2000, 0x2fff))    // nametable range
        buf = VRAM[NametableUnmirror(addr)];
    else if (IN_RANGE(addr, 0x3f00, 0x3fff))    // palette RAM indexes
        return ReadPaletteRAM(addr);
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
            addr -= 0x0010;
        paletteRAM[addr - 0x3f00] = val;
    }
}

uint8_t PPU::RegRead(uint16_t addr)
{
    switch (addr) {
    case PPU_REG_PPUSTATUS:
        genLatch = (genLatch & 0x1f) | (PPUSTATUS.raw & 0xe0);
        loopyW = 0;
        PPUSTATUS.vblankStarted = 0;
        break;
    case PPU_REG_PPUDATA:
        genLatch = MemRead(loopyV);
        if (!DuringRendering())
            loopyV += (PPUCTRL.vramIncrement) ? 32 : 1;
        else
            UpdateLoopyV();
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
        loopyT = (loopyT & 0x73ff) | ((static_cast<uint16_t>(val) & 0x03) << 10);
        PPUCTRL.raw = val;
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
        if (!loopyW) {
            loopyT = (loopyT & 0x7fe0) | static_cast<uint16_t>(val >> 3);
            loopyX = val & 0x07;
            loopyW = 1;
        } else if (loopyW == 1) {
            loopyT = (loopyT & 0x0c1f) | ((static_cast<uint16_t>(val) & 0x07) << 12) | ((static_cast<uint16_t>(val) >> 3) << 5);
            loopyW = 0;
        }
        break;
    case PPU_REG_PPUADDR:
        if (!loopyW) {
            loopyT = (loopyT & 0x40ff) | (static_cast<uint16_t>(val & 0x3f) << 8);
            loopyT = loopyT & ~(1U << 14);
            loopyW = 1;
        } else if (loopyW == 1) {
            loopyT = (loopyT & 0x7f00) | (static_cast<uint16_t>(val) & 0x00ff);
            loopyV = loopyT;
            loopyW = 0;
        }
        break;
    case PPU_REG_PPUDATA:
        MemWrite(loopyV, val);
        // if (scanLine <= 239)
            // printf("ppudata - tick: %d scanline: %d\n", tick, scanLine);
        if (!DuringRendering())
            loopyV += (PPUCTRL.vramIncrement) ? 32 : 1;
        else
            UpdateLoopyV();
        break;
    default:
        break;
    }    
    genLatch = val;
}


uint8_t PPU::ReadPaletteRAM(uint16_t addr) const
{
    auto IsAddrPaletteFirstEntry = [](uint16_t addr, uint8_t pal) -> bool {
        return (addr & 0x000f) == (pal * 4);
    };

    addr &= 0x3f1f;
    if (IsAddrPaletteFirstEntry(addr, 0) ||
        IsAddrPaletteFirstEntry(addr, 1) ||
        IsAddrPaletteFirstEntry(addr, 2) ||
        IsAddrPaletteFirstEntry(addr, 3))
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

void PPU::UpdateObjTable()
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            OAMEntry entry;
            entry.tileIndex = OAM[(i * 8 + j) * 4 + 1];
            entry.attribute.raw = OAM[(i * 8 + j) * 4 + 2];
            const uint16_t objPTAddr = 0x1000 * PPUCTRL.objPTAddr;
            const uint16_t tileAddr = objPTAddr + entry.tileIndex * 16;
            for (int k = 0; k < 8; k++) {
                const uint8_t lsb = pROM->chrROMRead(tileAddr + k);
                const uint8_t msb = pROM->chrROMRead(tileAddr + k + 8);
                for (int l = 0; l < 8; l++) {
                    const uint8_t pixel = (!entry.attribute.flipOBJHorizontally)
                                              ? NTHBIT(lsb, 7 - l) | (NTHBIT(msb, 7 - l) << 1)
                                              : NTHBIT(lsb, l) | (NTHBIT(msb, l) << 1);
                    objFrameBuffer[l + j * 8 + 64 * (k + 8 * i)] =
                        PALETTE[ReadPaletteRAM(0x3f00 + (pixel | (entry.attribute.palette << 2) | (1U << 4)))];
                }
            }
        }
    }
}

void PPU::UpdateTables()
{
    UpdatePatternTable();
    UpdateObjTable();
}

void PPU::GetTileData(int sL = -1)
{
    static unsigned step = 0;
    static uint8_t ntEntry, xOffset;
    int scanLine = (sL == -1) ? this->scanLine : sL;
    static uint8_t attribute, ptDataLow, ptDataHigh;

    switch (step) {
    case 0: // fetch nametable byte
        break;
    case 1:
        ntEntry = MemReadNoBuf(0x2000 | (loopyV & 0x0fff));
        // we set the x offset at first, then increment it later
        xOffset = loopyX;
        break;
    case 2: // fetch attribute table byte
        break;
    case 3:
        attribute = MemReadNoBuf(0x23c0 | (loopyV & 0x0c00) | ((loopyV >> 4) & 0x38) | ((loopyV >> 2) & 0x07));
        break;
    case 4: // get pattern table tile low
        break;
    case 5:
        ptDataLow = MemReadNoBuf(0x1000 * PPUCTRL.bgPTAddr + (ntEntry * 16) + (scanLine % 8));
        break;
    case 6: // get pattern table tile high
        break;
    case 7:
        ptDataHigh = MemReadNoBuf(0x1000 * PPUCTRL.bgPTAddr + (ntEntry * 16) + (scanLine % 8) + 8);
        // every 8 cycle, fed all data into the appropriate shift registers
        currentAttribute = nextAttribute;
        currentPTDataLow = nextPTDataLow;
        currentPTDataHigh = nextPTDataHigh;
        nextAttribute = attribute;
        nextPTDataLow = ptDataLow;
        nextPTDataHigh = ptDataHigh;

        // update coarse X
        if (RenderEnable()) {
            if ((loopyV & 0x001f) == 31) {
                loopyV &= ~0x001f;
                loopyV ^= 0x0400;
            } else {
                loopyV += 1;
                // printf("loopyV: %04x\n", loopyV);
            }
        }
        break;
    default:
        break;
    }

    if (IN_RANGE(tick, 1, 256) && IN_RANGE(this->scanLine, 0, SCREEN_HEIGHT - 1)) {
            // produce pixel
            const uint8_t pixel = NTHBIT(currentPTDataLow, 7 - step) | (NTHBIT(currentPTDataHigh, 7 - step) << 1);
            const uint16_t paletteEntry = (0x3f00 + pixel);
            screenFrameBuffer[this->scanLine * SCREEN_WIDTH + currentXPos] = PALETTE[ReadPaletteRAM(paletteEntry)];
            currentXPos++;
    }

    step = (step == 7) ? 0 : step + 1;
}

void PPU::UpdateLoopyV()
{
        // increment Y
    if ((loopyV & 0x7000) != 0x7000) {
         loopyV += 0x1000;
    } else {
        loopyV &= ~0x7000;
        int y = (loopyV & 0x03e0) >> 5;
        if (y == 29) {
            y = 0;
            loopyV ^= 0x0800;
        } else if (y == 31) {
            y = 0;
        } else {
            y++;
        }
        loopyV = (loopyV & ~0x03e0) | (y << 5);
    }
    // increment coarse X
    if ((loopyV & 0x001f) == 31) {
        loopyV &= ~0x001f;
        loopyV ^= 0x0400;
    } else {
        loopyV += 1;
    }
}

bool PPU::RenderEnable() const
{
    return PPUMASK.bgRenderingEnable || PPUMASK.objRenderingEnable;
}

bool PPU::DuringRendering() const
{
    return (RenderEnable() && ((scanLine == TOTAL_SCANLINE - 1) || (IN_RANGE(scanLine, 0, 239))));
}

void PPU::Step(int cycle)
{
    for (int i = 0; i < cycle * 3; i++) {
        if (IN_RANGE(tick, 1, 256)) {
            GetTileData();
            if (tick == 256 && RenderEnable()) {  // increment fine Y
                if ((loopyV & 0x7000) != 0x7000) {
                    loopyV += 0x1000;
                } else {
                    loopyV &= ~0x7000;
                    int y = (loopyV & 0x03e0) >> 5;
                    if (y == 29) {
                        y = 0;
                        loopyV ^= 0x0800;
                    } else if (y == 31) {
                        y = 0;
                    } else {
                        y++;
                    }
                    loopyV = (loopyV & ~0x03e0) | (y << 5);
                }
            }
        } else if (IN_RANGE(tick, 257, 320)) {
            if (RenderEnable() && tick == 257) {
                loopyV = (loopyV & ~0x041f) | (loopyT & 0x041f);
            } else if (RenderEnable() && IN_RANGE(tick, 280, 304) && scanLine == TOTAL_SCANLINE - 1) {
                loopyV = (loopyV & ~0x7be0) | (loopyT & 0x7be0);
            }
        } else if (IN_RANGE(tick, 321, 336)) {
            GetTileData((scanLine == TOTAL_SCANLINE - 1) ? 0 : scanLine + 1);
            // if (RenderEnable() && (tick == 328 || tick == 336)) {
            //     // increment coarse X
            //     if ((loopyV & 0x001f) == 31) {
            //         loopyV &= ~0x001f;
            //         loopyV ^= 0x0400;
            //     } else {
            //         loopyV += 1;
            //     }
            // }
        } else if (IN_RANGE(tick, 337, 340)) {

        }
        if (tick == 340) {
            tick = 0;
            currentXPos = 0;
            if (scanLine == SCREEN_HEIGHT - 1) {
                frameReady = true;
                // and vblank starts
                PPUSTATUS.vblankStarted = true;
            } else if (scanLine == TOTAL_SCANLINE - 1) {
                // vblank ends now
                PPUSTATUS.vblankStarted = false;
            }
            scanLine = (scanLine == TOTAL_SCANLINE - 1) ? 0 : scanLine + 1;
        } else {
            tick++;
        }
    }
    oldNMI = NMI;
    NMI = (PPUSTATUS.vblankStarted && PPUCTRL.vblankNMIEnable) ? false : true;
}

void PPU::Init(ROM *ROM, int _mirroring)
{
    mirrorring = static_cast<EMirroring>(_mirroring);
#if NESTEST
    tick = 21;
#else
    tick = 0;
#endif
    /* loopy registers init */
    loopyW = loopyT = loopyV = loopyX = 0;

    scanLine = 0;
    xPos = 0;
    genLatch = 0;
    frameReady = false;
    memset(ptFrameBuffer, 0, sizeof(ptFrameBuffer));
    memset(paletteRAM, 0, sizeof(paletteRAM));
    pROM = ROM;
    oldNMI = NMI = true;
    currentXPos = 0;
}

bool PPU::IsFrameReady() const { return frameReady; }
int PPU::GetScanline() const { return scanLine; }
int PPU::GetTicks() const { return tick; }
bool PPU::PullNMI() const { return (oldNMI && !NMI); }
void PPU::SetFrameNotReady() { frameReady = false; }
uint32_t * PPU::GetPTFrameBuffer() { return ptFrameBuffer; }
uint32_t * PPU::GetScreenFrameBuffer() { return screenFrameBuffer; }
uint8_t * PPU::GetOAM() { return OAM; }
uint32_t * PPU::GetOBJFrameBuffer() { return objFrameBuffer; }


/* constructor, destructor */
PPU::PPU() { }
PPU::~PPU() { }