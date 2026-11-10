#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdbool>

typedef class Cartridge {
public:
	bool load(const char* filename);
	Cartridge();
	~Cartridge();
} Cartridge;

#endif