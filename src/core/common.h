#ifndef COMMON_H_
#define COMMON_H_

#include <cstdint>
#include <stdio.h>

#define KiB                 1024
#define MiB                 1048576

#define MSB(n)              (((uint16_t)(n) >> 8) & 0x00ff)
#define LSB(n)              ((uint16_t)(n) & 0x00ff)
#define U16(lsb, msb)       (((uint16_t)(msb) << 8) | (uint16_t)(lsb))
#define NTHBIT(f, n)        (((f) >> (n)) & 0x0001)
#define SET(f, n)           ((f) |= (1U << (n)))
#define RES(f, n)           ((f) &= ~(1U << (n)))
#define IN_RANGE(x, a, b)   ((x) >= (a) && (x) <= (b))

#define CPU_RAM_SIZE        2 * KiB
#define PPU_RAM_SIZE        2 * KiB
#define PPU_PAL_SIZE        0x0020

#define PATTERN_TABLE_WIDTH         (16 * 8 * 2)
#define PATTERN_TABLE_HEIGHT        (16 * 8)
#define SCREEN_WIDTH_TILE           32
#define SCREEN_HEIGHT_TILE          30
#define SCREEN_WIDTH                (32 * 8)
#define SCREEN_HEIGHT               (30 * 8)
#define OBJ_TABLE_WIDTH             (8 * 8)
#define OBJ_TABLE_HEIGHT            (8 * 8)
#define TILE_MAP_WIDTH              SCREEN_WIDTH * 2
#define TILE_MAP_HEIGHT             SCREEN_HEIGHT * 2

#endif