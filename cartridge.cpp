#include <iostream>
#include <fstream>
#include "Cartridge.hpp"


bool Cartridge::load(const char* filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		return false;
	}
	return true;
}	

Cartridge::Cartridge() {
}

Cartridge::~Cartridge() {
}