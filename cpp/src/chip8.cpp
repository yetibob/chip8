#include <fstream>
#include <iostream>

#include "chip8.hpp"
#include "util.hpp"

#define PROGRAM_MEM_START 512

std::array<byte, 80> hexChars {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};


Chip8::Chip8(std::string rom): rom(rom), memory{}  {
	romPaths.push_back(".");
	romPaths.push_back("./roms");
	romPaths.push_back("${HOME}/.chip8/roms");

    init();
	load();
    printArr(memory);
}

void Chip8::init() {
    for (int i = 0; i < 512; i++) {
        memory[i] = hexChars[i];
    }
}

void Chip8::reset() {
    // do we need this mem reset???
    for (int i = 512; i < memory.size(); i++) {
        memory[i] = 0;
    }

    // wipe display
    for (auto& row : display) {
        for (auto& col : row) {
            col = 0;
        } 
    }
}

void Chip8::load() {
	std::ifstream romFile(rom, std::ifstream::binary);
	if (romFile) {
        // this is ugly...but i want my memory to be a byte[] (unsigned char) so am hacking here just cause....
        // should i be using this ugly ass cast thing or just a c style (char *) cast?
		romFile.read(reinterpret_cast<char *>(memory.data())+PROGRAM_MEM_START, fileLength(romFile));
	}
}

void Chip8::run() {
	while (true) {
		tick();
	}
}

void Chip8::tick() {}
