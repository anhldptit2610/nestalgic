#ifndef ROM_H_
#define ROM_H_

#include "common.h"
#include <iostream>
#include <fstream>


typedef struct {
    uint8_t raw[16];
    unsigned prgROMSize;
    unsigned chrROMSize;
    unsigned mapper;
    unsigned mirrorring;
} Header;

class ROM {
private:
    std::string name;
    Header header;
    uint8_t *prgROM = nullptr;
    uint8_t *chrROM = nullptr;

    int ParseHeader(std::ifstream&);
    void PrintROMInfo();
    void CopyROM(std::ifstream& , bool *);
    
    /* mapper 0 */
    uint8_t Mapper0Read(uint16_t);
    void Mapper0Write(uint16_t, uint8_t);
public:
    uint8_t chrROMRead(uint16_t);
    uint8_t prgROMRead(uint16_t);
    void prgROMWrite(uint16_t, uint8_t);
    int GetMirroringType();

    ROM(char *, bool *);
    ~ROM();
};


#endif