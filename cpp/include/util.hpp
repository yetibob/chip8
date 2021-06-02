#include <cstddef>
#include <array>
#include <iostream>

template <typename T, size_t size> 
void printArr(std::array<T, size> arr, int width = 80) {
	for (T el : arr) {
		std::cout << el << " ";
	}

	std::cout << std::endl;
}
