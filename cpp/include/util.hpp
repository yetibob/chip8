#include <array>
#include <fstream>
#include <iostream>

template <typename T, size_t size> 
void printArr(std::array<T, size> arr, int width = 80) {
	for (T el : arr) {
		std::cout << el << " ";
	}

	std::cout << std::endl;
}

int fileLength(std::ifstream& file) {
	file.seekg(0, file.end);
	int len = file.tellg();
	file.seekg(0, file.beg);
	return len;
}
