#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <map>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#define PROGRAM_MEM_START 0x200

#define D_WIDTH 64
#define D_HEIGHT 32

typedef uint8_t byte;

class Chip8 {
  public:
    Chip8(std::string rom, int scale);

    void run();
    void reset();
    bool load();

  private:
    std::array<byte, 0x1000> memory;
    std::array<uint16_t, 16> stack;
    std::array<byte, 16>     v;

    std::array<std::array<byte, D_WIDTH>, D_HEIGHT> display;

    std::vector<std::string> romPaths;
    std::map<byte, byte>     keys;

    byte sp, dt, st;

    uint16_t pc;
    uint16_t i;

    std::string rom;

    bool        loaded;
    bool        running;

    int         scale;

    SDL_Window*  window;
    SDL_Surface* surface;

    void init();
    void tick();
    void draw();
    void handleOp();
    void handleEvents();

    void handleTimers(double delta, double rate);

    byte waitForInput();
};

#endif
