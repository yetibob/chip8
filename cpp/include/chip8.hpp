#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <map>
#include <string>
#include <vector>

#include "SDL.h"

#define D_WIDTH 64
#define D_HEIGHT 32

typedef unsigned char byte;

class Chip8 {
  public:
    std::string rom;

    Chip8(std::string rom);

    void run(int s);
    void reset();
    void load();

  private:
    std::array<byte, 0x1000> memory;
    std::array<uint16_t, 16> stack;
    std::array<byte, 16>     v;

    std::array<std::array<byte, D_WIDTH>, D_HEIGHT> display;

    std::vector<std::string> romPaths;
    std::map<byte, bool>     keys;

    byte sp, dt, st;

    uint16_t pc;
    uint16_t i;

    int  scale;
    bool running;

    SDL_Window*  window;
    SDL_Surface* surface;

    void init();
    void tick();
    void handleOp();
    void draw();

    byte waitForInput();
};

#endif
