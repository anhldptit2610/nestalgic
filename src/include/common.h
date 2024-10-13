#ifndef COMMON_H_
#define COMMON_H_

#include <cstdint>
#include <iostream>
#include <stdio.h>

#define KiB             1024
#define MiB             1048576

#define MSB(n)              (((uint16_t)(n) >> 8) & 0x00ff)
#define LSB(n)              ((uint16_t)(n) & 0x00ff)
#define U16(lsb, msb)       (((uint16_t)(msb) << 8) | (uint16_t)(lsb))
#define NTHBIT(f, n)        (((f) >> (n)) & 0x0001)
#define SET(f, n)           ((f) |= (1U << (n)))
#define RES(f, n)           ((f) &= ~(1U << (n)))
#define IN_RANGE(x, a, b)   ((x) >= (a) && (x) <= (b))

#endif