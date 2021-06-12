#include <array>
#include <fstream>
#include <iostream>

template <typename T, size_t size> 
void printArr(std::array<T, size> arr, int width = 80) {
	for (auto el : arr) {
		std::cout << el << " ";
	}

	std::cout << std::endl;
}

template <typename T1, typename T2> 
void printArr(std::map<T1, T2> m) {
	for (auto [key, value] : m) {
		std::cout << key << " : " << value << std::endl;
	}
}
int fileLength(std::ifstream& file) {
	file.seekg(0, file.end);
	int len = file.tellg();
	file.seekg(0, file.beg);
	return len;
}
