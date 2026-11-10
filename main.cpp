#include "nestalgic.hpp"
#include "log.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    NESTALGIC_LOG::init();
    Nestalgic nestalgic;

	NESTALGIC_LOG::log("Initializing Nestalgic...");
    return 0;
}