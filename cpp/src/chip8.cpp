#include <fstream>
#include <iostream>

#include "chip8.hpp"
#include "util.hpp"

int fileLength(std::ifstream& file) {
	file.seekg(0, file.end);
	int len = file.tellg();
	file.seekg(0, file.beg);
	return len;
}


Chip8::Chip8(std::string rom): rom(rom), memory{}  {
	romPaths.push_back(".");
	romPaths.push_back("./roms");
	romPaths.push_back("${HOME}/.chip8/roms");

	load();

	printArr(memory);
}

void Chip8::run() {
	while (true) {
		tick();
	}
}

void Chip8::reset() {}

void Chip8::load() {
	std::ifstream romFile(rom, std::ifstream::binary);
	if (romFile) {
		romFile.read(memory.data(), fileLength(romFile));
	}
}

void Chip8::tick() {}
