#include "ppu.h"
#include <cstring>

#define TOTAL_SCANLINE          262
#define PRE_RENDER_SCANLINE     TOTAL_SCANLINE - 1

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

uint8_t PPU::GetAttributeTableEntry(unsigned nametable, unsigned x, unsigned y) const
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
        genLatch = MemRead(loopyV.raw);
        if (!DuringRendering())
            loopyV.raw += (PPUCTRL.vramIncrement) ? 32 : 1;
        else if (DuringRendering())
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
        loopyT.raw = (loopyT.raw & 0x73ff) | ((static_cast<uint16_t>(val) & 0x03) << 10);
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
            loopyT.raw = (loopyT.raw & 0x7fe0) | ((static_cast<uint16_t>(val) & 0xfc) >> 3);
            loopyX = val & 0x07;
            loopyW = 1;
        } else if (loopyW == 1) {
            loopyT.raw = (loopyT.raw & 0x0c1f) | ((static_cast<uint16_t>(val) & 0x07) << 12) | ((static_cast<uint16_t>(val) >> 3) << 5);
            loopyW = 0;
        }
        break;
    case PPU_REG_PPUADDR:
        if (!loopyW) {
            loopyT.raw = (loopyT.raw & 0x40ff) | (static_cast<uint16_t>(val & 0x3f) << 8);
            loopyT.raw = loopyT.raw & ~(1U << 14);
            loopyW = 1;
        } else if (loopyW == 1) {
            loopyT.raw = (loopyT.raw & 0x7f00) | (static_cast<uint16_t>(val) & 0x00ff);
            loopyV = loopyT;
            loopyW = 0;
        }
        break;
    case PPU_REG_PPUDATA:
        MemWrite(loopyV.raw, val);
        // if (scanLine <= 239)
            // printf("ppudata - tick: %d scanline: %d\n", tick, scanLine);
        if (!DuringRendering())
            loopyV.raw += (PPUCTRL.vramIncrement) ? 32 : 1;
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

void PPU::UpdateTileMap()
{
    for (int i = 0; i < SCREEN_HEIGHT_TILE; i++) {
        for (int j = 0; j < SCREEN_WIDTH_TILE; j++) {
            DrawBgTile(0, j, i);
        }
    }

    for (int i = 0; i < SCREEN_HEIGHT_TILE; i++) {
        for (int j = 0; j < SCREEN_WIDTH_TILE; j++) {
            DrawBgTile(1, j, i);
        }
    }

    // bottom left tile map
    for (int i = 0; i < SCREEN_HEIGHT_TILE; i++) {
        for (int j = 0; j < SCREEN_WIDTH_TILE; j++) {
            DrawBgTile(2, j, i);
        }
    }

    // bottom right tile map
    for (int i = 0; i < SCREEN_HEIGHT_TILE; i++) {
        for (int j = 0; j < SCREEN_WIDTH_TILE; j++) {
            DrawBgTile(3, j, i);
        }
    }
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


void PPU::DrawBgTile(uint16_t base, uint8_t x, uint8_t y)
{
    uint8_t tileIndex, attribute, firstPlane, secondPlane, pixel;
    uint16_t baseAddr = 0x2000 + 0x0400 * base, frameXOffset, frameYOffset, ptBaseAddr = 0x1000 * PPUCTRL.bgPTAddr;

    tileIndex = MemReadNoBuf(baseAddr + y * SCREEN_WIDTH_TILE + x);
    attribute = GetAttributeTableEntry(base, x, y);

    if (base == 0) {
        frameXOffset = 0;
        frameYOffset = 0;
    } else if (base == 1) {
        frameXOffset = SCREEN_WIDTH;
        frameYOffset = 0;
    } else if (base == 2) {
        frameXOffset = 0;
        frameYOffset = SCREEN_HEIGHT;
    } else if (base == 3) {
        frameXOffset = SCREEN_WIDTH;
        frameYOffset = SCREEN_HEIGHT;
    }

    attribute = (attribute >> (((y % 4) / 2 * 2 + (x % 4) / 2) * 2)) & 0x03;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            firstPlane = MemReadNoBuf(ptBaseAddr + tileIndex * 16 + i); 
            secondPlane = MemReadNoBuf(ptBaseAddr + tileIndex * 16 + i + 8); 
            pixel = NTHBIT(firstPlane, 7 - j) | (NTHBIT(secondPlane, 7 - j) << 1);
            tileMapFrameBuffer[(frameYOffset + y * 8 + i) * SCREEN_WIDTH * 2 + frameXOffset + x * 8 + j] = PALETTE[ReadPaletteRAM(0x3f00 + (pixel | (attribute << 2)))];
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
    UpdateTileMap();
    UpdatePatternTable();
    UpdateObjTable();
}

void PPU::ProcessPixel(int sL = -1)
{
    auto GetPaletteNumber = [this](uint8_t attribute) -> uint8_t {
        uint8_t x = this->currentRenderData.x;
        uint8_t y = this->currentRenderData.y;
        return (attribute >> (((y % 4) / 2 * 2 + (x % 4) / 2) * 2)) & 0x03;
    };

    static unsigned step = 0;
    static uint8_t ntEntry;
    static RenderData renderData;

    // TODO: seperate this from above code
    if (IN_RANGE(tick, 1, 256) && IN_RANGE(this->scanLine, 0, SCREEN_HEIGHT - 1)) {
            const uint8_t pixel = NTHBIT(currentRenderData.ptLow, 7 - ((tick - 1) % 8)) | (NTHBIT(currentRenderData.ptHigh, 7 - ((tick - 1) % 8)) << 1);
            const uint16_t paletteEntry = (0x3f00 | pixel | (GetPaletteNumber(currentRenderData.attribute) << 2));
            screenFrameBuffer[this->scanLine * SCREEN_WIDTH + currentXPos] = PALETTE[ReadPaletteRAM(paletteEntry)];
            currentXPos = (currentXPos == SCREEN_WIDTH - 1) ? 0 : currentXPos + 1;
    }

    switch ((tick - 1) % 8) {
    case 0:
        break;
    case 1:
        ntEntry = MemReadNoBuf(0x2000 | (loopyV & 0x0fff));
        renderData.y = loopyV.CoarseY();
        renderData.x = loopyV.CoarseX();
        break;
    case 2: // fetch attribute table byte
        break;
    case 3:
        renderData.attribute = GetAttributeTableEntry(loopyV.NT(), loopyV.CoarseX(), loopyV.CoarseY());
        break;
    case 4: // get pattern table tile low
        break;
    case 5:
        renderData.ptLow = MemReadNoBuf(0x1000 * PPUCTRL.bgPTAddr + (ntEntry * 16) + loopyV.FineY());
        break;
    case 6: // get pattern table tile high
        break;
    case 7:
        renderData.ptHigh = MemReadNoBuf(0x1000 * PPUCTRL.bgPTAddr + (ntEntry * 16) + loopyV.FineY() + 8);
        // update coarse X
        if (IsRenderEnable())
            IncrementX();
        currentRenderData = nextRenderData;
        nextRenderData = renderData;
        break;
    default:
        break;
    }

    step = (step + 1) % 8;
}

void PPU::UpdateLoopyV()
{
    IncrementY();
    IncrementX();
}

void PPU::IncrementX()
{
    if ((loopyV & 0x001f) == 31) {
        loopyV &= ~0x001f;
        loopyV ^= 0x0400;
    } else {
        loopyV += 1;
    }
}

void PPU::IncrementY()
{
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

bool PPU::IsRenderEnable() const
{
    return PPUMASK.bgRenderingEnable || PPUMASK.objRenderingEnable;
}

bool PPU::DuringRendering() const
{
    return (IsRenderEnable() && ((scanLine == PRE_RENDER_SCANLINE) || (IN_RANGE(scanLine, 0, 239))));
}

void PPU::Step(int cpuCycle)
{
    for (int i = 0; i < cpuCycle * 3; i++) {
        if (IN_RANGE(this->scanLine, 0, 239) || this->scanLine == PRE_RENDER_SCANLINE) {
            if (tick == 0) {

            } else if (IN_RANGE(tick, 1, 256)) {
                if (this->scanLine == PRE_RENDER_SCANLINE && tick == 1)
                    PPUSTATUS.vblankStarted = false;
                ProcessPixel();
                if (tick == 256 && IsRenderEnable()) {
                    IncrementY();
                }
            } else if (IN_RANGE(tick, 257, 320)) {
                if (IsRenderEnable() && tick == 257) {
                    loopyV = (loopyV & 0x7be0) | (loopyT & 0x041f);
                } else if (IsRenderEnable() && IN_RANGE(tick, 280, 304) && scanLine == PRE_RENDER_SCANLINE) {
                    loopyV = (loopyV & 0x041f) | (loopyT & 0x7be0);
                }
            } else if (IN_RANGE(tick, 321, 336)) {
                ProcessPixel((scanLine == PRE_RENDER_SCANLINE) ? 0 : scanLine + 1);
            } else if (IN_RANGE(tick, 337, 340)) {

            }
        } else if (this->scanLine == 240) {     // idle this scanline

        } else if (this->scanLine > 240) {
            if (this->scanLine == 241 && tick == 1)
                PPUSTATUS.vblankStarted = true;
        }

        if (tick == 340) {
            tick = 0;
            currentXPos = 0;
            frameReady = (scanLine == SCREEN_HEIGHT - 1) ? true : false;
            scanLine = (scanLine == PRE_RENDER_SCANLINE) ? 0 : scanLine + 1;
        } else {
            tick++;
        }
    }
    oldNMI = NMI;
    NMI = !(PPUSTATUS.vblankStarted && PPUCTRL.vblankNMIEnable);
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
    loopyW = loopyX = 0;
    loopyT = loopyV = 0;

    scanLine = 0;
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
uint32_t * PPU::GetTileMapFrameBuffer() { return tileMapFrameBuffer; }
uint32_t * PPU::GetScreenFrameBuffer() { return screenFrameBuffer; }
uint8_t * PPU::GetOAM() { return OAM; }
uint32_t * PPU::GetOBJFrameBuffer() { return objFrameBuffer; }


/* constructor, destructor */
PPU::PPU() { }
PPU::~PPU() { }