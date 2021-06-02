#include <array>
#include <string>
#include <vector>

typedef char byte;

class Chip8 {
public:
	std::string rom;

	Chip8(std::string rom);

	void run();
	void reset();
	void load();
private:
	std::array<byte, 0x1000> memory; // 4096 bytes 
	std::vector<std::string> romPaths;

	void tick();
};
