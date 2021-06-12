#include <array>
#include <map>
#include <string>
#include <vector>

#include "SDL.h"

typedef unsigned char byte;

class Chip8 {
public:
	std::string rom;

	Chip8(std::string rom);

	void run(int s);
	void reset();
	void load();
private:
	std::array<byte,     0x1000> memory;
    std::array<uint16_t, 16>     stack;
    std::array<byte,     16>     v;

    std::array<std::array<byte, 64>, 32> display; // display, 64x32 (colxrow)

	std::vector<std::string> romPaths;
	std::map<byte, bool>     keys;


    byte sp, dt, st; // stack pointer, delay timer, sound timer

    uint16_t pc; // program counter
    uint16_t i;  // 16 bit i register. Normally used to store mem addresses

    int      scale;   // pixel and display scaling
    bool     running;

    SDL_Window *window;
    SDL_Surface *surface;

    void init();
    void tick();
    void handleOp();
	void draw();

	byte waitForInput();
};
