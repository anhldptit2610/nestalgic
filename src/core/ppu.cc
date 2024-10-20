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
        w = 0;
        PPUSTATUS.vblankStarted = 0;
        break;
    case PPU_REG_PPUDATA:
        genLatch = MemRead(v);
        v += (PPUCTRL.vramIncrement) ? 32 : 1;
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
        t = (t & 0xf3ff) | ((static_cast<uint16_t>(val) & 0x03) << 10);
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
        if (!w) {
            t = (t & 0xffe0) | static_cast<uint16_t>(val >> 3);
            x = val & 0x07;
            w = 1;
        } else if (w == 1) {
            t = (t & 0x8c1f) | ((static_cast<uint16_t>(val) & 0x07) << 12) | ((static_cast<uint16_t>(val) >> 3) << 5);
            w = 0;
        }
        break;
    case PPU_REG_PPUADDR:
        if (!w) {
            t = (t & 0xc0ff) | (static_cast<uint16_t>(val & 0x3f) << 8);
            t = t & ~(1U << 14);
            w = 1;
        } else if (w == 1) {
            t = (t & 0xff00) | (static_cast<uint16_t>(val) & 0x00ff);
            v = t;
            w = 0;
        }
        break;
    case PPU_REG_PPUDATA:
        MemWrite(v, val);
        v += (PPUCTRL.vramIncrement) ? 32 : 1;
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

void PPU::DrawBackgroundOnScanline()
{
    if (PPUMASK.bgRenderingEnable) {
        uint8_t nametable = 0, scanLine,
            xPos;

        // X scroll handling
        if (this->xScroll >= SCREEN_WIDTH) {
            if (this->nametable == 0 || this->nametable == 2)
                this->nametable += 1;
            else if (this->nametable == 1 || this->nametable == 3)
                this->nametable -= 1;
            this->xScroll -= SCREEN_WIDTH;
            xPos = this->xScroll - SCREEN_WIDTH;
        } else {
            xPos = this->xScroll;
            nametable = this->nametable;
        }
        // Y scroll handling
        if (this->yScroll + this->scanLine >= SCREEN_HEIGHT) {
            if (nametable == 0 || nametable == 1)
                nametable = this->nametable + 2;
            else if (nametable == 2 || nametable == 3)
                nametable = this->nametable - 2;
            scanLine = this->yScroll + this->scanLine - 240;
        } else {
            nametable = this->nametable;
            scanLine = this->yScroll + this->scanLine;
        }
        for (int k = 0; k < SCREEN_WIDTH;) {
            const uint16_t ntBaseAddr = 0x2000 + 0x0400 * nametable;
            const uint8_t ntEntry = MemReadNoBuf(ntBaseAddr + (scanLine / 8) * SCREEN_WIDTH_TILE + xPos / 8);
            uint8_t atEntry = GetAttributeTableEntry(nametable, xPos / 8, scanLine / 8);
            atEntry = (atEntry >> ((((scanLine / 8) % 4) / 2 * 2 + ((xPos / 8) % 4) / 2) * 2)) & 0x03;
            const uint16_t tileBaseAddr = (0x1000 * PPUCTRL.bgPTAddr) + (ntEntry * 16);
            const uint8_t firstPlane = MemReadNoBuf(tileBaseAddr + (scanLine % 8));
            const uint8_t secondPlane = MemReadNoBuf(tileBaseAddr + (scanLine % 8) + 8);
            const uint8_t tileRenderStart = xPos % 8;
            const uint8_t tileRenderSize = 8 - (xPos % 8);

            for (int j = tileRenderStart; j < tileRenderStart + tileRenderSize; j++) {
                const uint8_t pixel = NTHBIT(firstPlane, 7 - j) | (NTHBIT(secondPlane, 7 - j) << 1);
                const uint16_t paletteEntry = (0x3f00 + (pixel | (atEntry << 2)));
                screenFrameBuffer[this->scanLine * SCREEN_WIDTH + k] = PALETTE[ReadPaletteRAM(paletteEntry)];
                k++;
            }
            if (xPos + tileRenderSize >= SCREEN_WIDTH) {
                xPos = xPos + tileRenderSize - SCREEN_WIDTH;
                if (nametable == 0 || nametable == 2)
                    nametable += 1;
                else if (nametable == 1 || nametable == 3)
                    nametable -= 1;
            } else {
                xPos += tileRenderSize;
            }
        }
    }
}

void PPU::DrawSpriteOnScanline()
{
    OAMEntry oamMemory[8];

    if (!PPUMASK.objRenderingEnable)
        return;

    // we search for 8 sprites whose y position is equal to the scanline.
    uint8_t index = 0;
    for (int i = 0; i < 64 && index < 8; i++) {
        if (IN_RANGE(this->scanLine, this->OAM[i * 4], this->OAM[i * 4] + 7)) {
            oamMemory[index].yPos = this->OAM[i * 4];
            oamMemory[index].tileIndex = this->OAM[i * 4 + 1];
            oamMemory[index].attribute.raw = this->OAM[i * 4 + 2];
            oamMemory[index].xPos = this->OAM[i * 4 + 3];
            index++;
        }
    }

    // then draw all of them into the scanline
    for (int i = 0; i < index; i++) {
        uint8_t firstPlane, secondPlane;
        const uint16_t tileBaseAddr = (0x1000 * PPUCTRL.objPTAddr) + oamMemory[i].tileIndex * 16;
        if (!oamMemory[i].attribute.flipOBJVertically) {
            firstPlane = MemReadNoBuf(tileBaseAddr + (this->scanLine - oamMemory[i].yPos));
            secondPlane = MemReadNoBuf(tileBaseAddr + (this->scanLine - oamMemory[i].yPos) + 8);
        } else {
            firstPlane = MemReadNoBuf(tileBaseAddr + (7 - this->scanLine + oamMemory[i].yPos));
            secondPlane = MemReadNoBuf(tileBaseAddr + (7 - this->scanLine + oamMemory[i].yPos) + 8);
        }
        for (int j = 0; j < 8; j++) {
            const uint8_t bit = (oamMemory[i].attribute.flipOBJHorizontally) ? j : 7 - j;
            const uint8_t pixel = NTHBIT(firstPlane, bit) | (NTHBIT(secondPlane, bit) << 1);
            const uint16_t paletteEntry = (0x3f00 + (pixel | (oamMemory[i].attribute.palette << 2) | (1U << 4)));
            if (pixel > 0)
                screenFrameBuffer[this->scanLine * SCREEN_WIDTH + oamMemory[i].xPos + j] = PALETTE[MemReadNoBuf(paletteEntry)];
        }
    }
}

void PPU::DrawScanline()
{
    DrawBackgroundOnScanline();
    DrawSpriteOnScanline();
}

void PPU::Step(int cycle)
{
    tick += cycle * 3;
    while (tick > 340) {
        if (scanLine < 240)
            DrawScanline();
        tick -= 340;
        yScroll = ((t >> 5) & 0x001f) * 8 + ((t >> 12) & 0x0007);
        xScroll = (t & 0x001f) * 8 + x;
        nametable = (t >> 10) & 0x03;
        if (scanLine == SCREEN_HEIGHT - 1) {
            frameReady = true;
            // and vblank starts
            PPUSTATUS.vblankStarted = true;
        } else if (scanLine == TOTAL_SCANLINE - 1) {
            // vblank ends now
            PPUSTATUS.vblankStarted = false;
            PPUSTATUS.obj0Hit = false;
        }
        scanLine = (scanLine == TOTAL_SCANLINE - 1) ? 0 : scanLine + 1;
        if (scanLine == 30)
            PPUSTATUS.obj0Hit = true;
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
    w = 0;
    scanLine = 0;
    genLatch = 0;
    frameReady = false;
    memset(ptFrameBuffer, 0, sizeof(ptFrameBuffer));
    memset(paletteRAM, 0, sizeof(paletteRAM));
    pROM = ROM;
    oldNMI = NMI = true;
    yScroll = xScroll = 0;
    nametable = 0;
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