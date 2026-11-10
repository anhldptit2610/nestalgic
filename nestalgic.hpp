#ifndef NESTALGIC_HPP
#define NESTALGIC_HPP

#include "cartridge.hpp"
#include <cstdbool>

typedef class Nestalgic {
private:
	Cartridge cartridge;
public:
	bool init(const char*);
    Nestalgic();
    ~Nestalgic();
} Nestalgic;


#endif