#include <fstream>
#include <iostream>
#include <random>

#include "SDL.h"

#include "chip8.hpp"
#include "util.hpp"

#define PROGRAM_MEM_START 512

std::array<byte, 80> hexChars{
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

std::map<SDL_Scancode, byte> keymap{
    { SDL_SCANCODE_1, 0x1 }, { SDL_SCANCODE_2, 0x2 }, { SDL_SCANCODE_3, 0x3 },
    { SDL_SCANCODE_4, 0xC }, { SDL_SCANCODE_Q, 0x4 }, { SDL_SCANCODE_W, 0x5 },
    { SDL_SCANCODE_E, 0x6 }, { SDL_SCANCODE_R, 0xD }, { SDL_SCANCODE_A, 0x7 },
    { SDL_SCANCODE_S, 0x8 }, { SDL_SCANCODE_D, 0x9 }, { SDL_SCANCODE_F, 0xE },
    { SDL_SCANCODE_Z, 0xA }, { SDL_SCANCODE_X, 0x0 }, { SDL_SCANCODE_C, 0xB },
    { SDL_SCANCODE_V, 0xF }
};

std::default_random_engine         gen;
std::uniform_int_distribution<int> dist(0, 255);

Chip8::Chip8(std::string rom) : rom(rom), memory{}, display{} {
    romPaths.push_back(".");
    romPaths.push_back("./roms");
    romPaths.push_back("${HOME}/.chip8/roms");

    init();
    load();
}

void Chip8::init() {
    for (int i = 0; i < 512; i++) {
        memory[i] = hexChars[i];
    }

    keys[0x1] = false;
    keys[0x2] = false;
    keys[0x3] = false;
    keys[0xC] = false;
    keys[0x4] = false;
    keys[0x5] = false;
    keys[0x6] = false;
    keys[0xD] = false;
    keys[0x7] = false;
    keys[0x8] = false;
    keys[0x9] = false;
    keys[0xE] = false;
    keys[0xA] = false;
    keys[0x0] = false;
    keys[0xB] = false;
    keys[0xF] = false;

    reset();
}

void Chip8::reset() {
    pc = 0x200;
    sp = 0;
    dt = 0;
    st = 0;

    for (auto& row : display) {
        for (auto& col : row) {
            col = 0;
        }
    }

    for (auto& [key, value] : keys) {
        value = false;
    }
}

void Chip8::load() {
    std::ifstream romFile{ rom, std::ifstream::binary };
    if (romFile) {
        romFile.read(reinterpret_cast<char*>(memory.data()) + PROGRAM_MEM_START,
                     fileLength(romFile));
    }
}

void Chip8::run(int s) {
    scale   = s;
    running = true;

    SDL_Init(SDL_INIT_VIDEO & SDL_INIT_EVENTS);

    window  = SDL_CreateWindow("Chip8",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              64 * scale,
                              32 * scale,
                              SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);

    SDL_Event event{};
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    auto item{ keymap.find(event.key.keysym.scancode) };
                    if (item != keymap.end()) {
                        keys[item->first] = true;
                    }
                    break;
            }
        }
        tick();
        draw();
    }
}

void Chip8::tick() {
    uint16_t op;
    op = (memory[pc] << 8) | memory[pc + 1];

    // std::cout << "State:\n";
    // std::cout << "\tOP: 0x" << std::hex << op << "\n";
    // std::cout << "\tPC: 0x" << std::hex << pc << std::dec << std::endl;

    pc += 2;

    uint16_t addr = op & 0xFFF;

    byte x  = (op & 0xF00) >> 8;
    byte y  = (op & 0xF0) >> 4;
    byte kk = (op & 0xFF);
    byte n  = (op & 0xF);

    switch (op >> 12) {
        case 0x0:
            switch (op & 0xFF) {
                case 0xE0:
                    for (auto& row : display) {
                        for (auto& pix : row) {
                            pix = 0x0;
                        }
                    }
                    break;
                case 0xEE:
                    pc = stack[sp--];
                    break;
            }
            break;
        case 0x1:
            pc = addr;
            break;
        case 0x2:
            sp++;
            stack[sp] = pc;
            pc        = addr;
            break;
        case 0x3:
            if (v[x] == kk) {
                pc += 2;
            }
            break;
        case 0x4:
            if (v[x] != kk) {
                pc += 2;
            }
            break;
        case 0x5:
            if (v[x] == v[y]) {
                pc += 2;
            }
            break;
        case 0x6:
            v[x] = kk;
            break;
        case 0x7:
            v[x] += kk;
            break;
        case 0x8:
            switch (op & 0xF) {
                case 0x0:
                    v[x] = v[y];
                    break;
                case 0x1:
                    v[x] |= v[y];
                    break;
                case 0x2:
                    v[x] &= v[y];
                    break;
                case 0x3:
                    v[x] ^= v[y];
                    break;
                case 0x4: {
                    uint16_t tmp = v[x] + v[y];
                    if (tmp > 255) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[x] = tmp;
                    break;
                }
                case 0x5:
                    if (v[x] > v[y]) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[x] -= v[y];
                    break;
                case 0x6:
                    v[0xF] = v[x] & 0x1;
                    v[x] /= 2;
                    break;
                case 0x7:
                    if (v[y] > v[x]) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[x] = v[y] - v[x];
                    break;
                case 0xE:
                    v[0xF] = v[x] >> 7;
                    v[x]   = v[x] << 1;
                    break;
            }
        case 0x9:
            if (v[x] != v[y]) {
                pc += 2;
            }
            break;
        case 0xA:
            i = addr;
            break;
        case 0xB:
            pc = addr + v[0];
            break;
        case 0xC: {
            v[x] = dist(gen);
            break;
        }
        case 0xD: {
            bool erased{ false };

            for (byte x = 0; x < n; x++) {
                byte loc_y = v[y] + x;
                if (loc_y > 31) {
                    loc_y -= 31;
                }

                byte sprite = memory[i + x];
                byte oldSprite{};

                // Mash together display into single byte for xoring
                for (byte y; y < 8; y++) {
                    byte loc_x = v[x] + y;
                    if (loc_x > 63) {
                        loc_x -= 63;
                    }

                    oldSprite = oldSprite | display[loc_y][loc_x];
                    // do not bit shift left on final op, this causing a pixel
                    // to be lost
                    if (y < 7) {
                        oldSprite = oldSprite << 1;
                    }
                }

                sprite = sprite ^ oldSprite;

                // break sprite back up into separate display bytes
                // we use j != 255 because we are dealing with a uint
                // and uints wrap around back to the top when they go below zero
                // so j >= 0 would always hold true
                for (byte y = 7; y != 255; y--) {
                    byte loc_x = v[x] + y;
                    if (loc_x > 63) {
                        loc_x -= 63;
                    }

                    byte tmp = display[loc_y][loc_x];

                    display[loc_y][loc_x] = sprite & 0x1;
                    // it doesn't matter here that we go one to far with bit
                    // shift sprite because it won't be used after the last call
                    // anyway
                    sprite = sprite >> 1;

                    if (!erased && tmp == 0x1 && display[loc_y][loc_x] == 0x0) {
                        v[0xF] = 1;
                        erased = true;
                    }
                }
            }

            if (!erased) {
                v[0xF] = 0;
            }
            break;
        }
        case 0xE:
            switch (op & 0xFF) {
                case 0x9E:
                    if (keys[v[x]]) {
                        pc += 2;
                        keys[v[x]] = false;
                    }
                    break;
                case 0xA1:
                    if (!keys[v[x]]) {
                        pc += 2;
                    } else {
                        keys[v[x]] = false;
                    }
                    break;
            }
            break;
        case 0xF:
            switch (op & 0xFF) {
                case 0x07:
                    v[x] = dt;
                    break;
                case 0x0A:
                    v[x] = waitForInput();
                    break;
                case 0x15:
                    dt = v[x];
                    break;
                case 0x18:
                    st = v[x];
                    break;
                case 0x1E:
                    i += v[x];
                    break;
                case 0x29:
                    i = v[x] * 5;
                    break;
                case 0x33: {
                    uint32_t bcd = v[x];

                    // double dabble algorithm for binary to bcd
                    // https://en.wikipedia.org/wiki/Double_dabble
                    // we can hardcode our limit to 8 since chip8 registers are
                    // 8 bits in length
                    for (short i = 0; i < 8; i++) {
                        // Check if hundreds column is greater than 4. If so,
                        // add 3 to hundreds column
                        if (((bcd & 0xF0000) >> 16) > 4) {
                            bcd = (((bcd >> 16) + 3) << 16) | (bcd & 0xFFFF);
                        }

                        // Check if tens column is greater than 4. If so, add 3
                        // to tens column
                        if (((bcd & 0xF000) >> 12) > 4) {
                            bcd = (((bcd >> 12) + 3) << 12) | (bcd & 0xFFF);
                        }

                        // Check if ones column is greater than 4. If so, add 3
                        // to ones column
                        if (((bcd & 0xF00) >> 8) > 4) {
                            bcd = (((bcd >> 8) + 3) << 8) | (bcd & 0xFF);
                        }

                        bcd = bcd << 1;
                    }

                    memory[i]     = (bcd & 0xF0000) >> 16;
                    memory[i + 1] = (bcd & 0xF000) >> 12;
                    memory[i + 2] = (bcd & 0xF00) >> 8;
                    break;
                }
                case 0x55:
                    // TODO: Make sure this stuff is right
                    for (uint16_t j = 0x0; j <= x; j++) {
                        memory[i + j] = v[j];
                    }
                    break;
                case 0x65:
                    for (uint16_t j = 0x0; j <= x; j++) {
                        v[j] = memory[i + j];
                    }
                    break;
            }
            break;
        default:
            std::cout << "UNKNOWN OPCODE 0x" << std::hex << op << std::dec
                      << std::endl;
            running = false;
    }
}

byte Chip8::waitForInput() {
    SDL_Event event;

    while (SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                return 0xFF;
            case SDL_KEYDOWN:
                auto item = keymap.find(event.key.keysym.scancode);
                if (item != keymap.end()) {
                    return item->first;
                }
        }
    }
    return 0xFF; // should never happen
}

// this isn't great tbh. i wish i could just draw directly to the screen
// and scale pixel sizes as needed. such is life
bool                                                i;
std::array<std::array<SDL_Rect, D_WIDTH>, D_HEIGHT> rects;

void Chip8::draw() {
    if (!i) {
        for (int y = 0; y < D_HEIGHT; y++) {
            for (int x = 0; x < D_WIDTH; x++) {
                rects[y][x] = SDL_Rect{ scale * x, scale * y, scale, scale };
            }
        }
        i = true;
    }

    SDL_FillRect(surface, NULL, 0);

    for (int y = 0; y < D_HEIGHT; y++) {
        for (int x = 0; x < D_WIDTH; x++) {
            if (display[y][x] == 0x1) {
                SDL_FillRect(surface,
                             &rects[y][x],
                             SDL_MapRGB(surface->format, 0, 255, 255));
            }
        }
    }

    SDL_UpdateWindowSurface(window);
}
