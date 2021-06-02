#include <iostream>

#include "chip8.hpp"

int main() {
	std::string defaultRom = "../../roms/INVADERS";

	Chip8 chip8{defaultRom};
}
