#include <array>
#include <map>
#include <string>
#include <vector>

typedef unsigned char byte;

class Chip8 {
public:
	std::string rom;

	Chip8(std::string rom);

	void run();
	void reset();
	void load();
private:
    // Chip8 specific
	std::array<byte, 0x1000> memory; // 4096 bytes 
    std::array<uint16_t, 16> stack; // Stack
    std::array<byte, 16> v; // V0-VF registers
    std::array<std::array<byte, 64>, 32> display; // display, 64x32 (colxrow)

    byte pc; // program counter
    byte sp; // stack pointer
    byte dt, st; // delay timer, sound timer

    uint16_t i; // 16 bit i register. Normally used to store mem addresses

    // Extra stuff
	std::vector<std::string> romPaths;

    uint32_t scale; // pixel and display scaling
    bool running;

	std::map<byte, bool> keys;

    void init();
    void tick();
    void handleOp();
};
