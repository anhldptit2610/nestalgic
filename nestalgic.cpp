#include "nestalgic.hpp"

bool Nestalgic::init(const char* filename) {
	return cartridge.load(filename);
}

Nestalgic::Nestalgic() {
}

Nestalgic::~Nestalgic() {
}