#include <iostream>

#include "chip8.hpp"

int main() {
    int         scale      = 15;
    std::string defaultRom = "INVADERS";

    Chip8 chip8{ defaultRom, scale };
    chip8.run();
}
