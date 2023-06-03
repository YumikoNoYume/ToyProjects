#pragma once

#include <cstddef>
#include <random>

namespace ArrayDetails {
	inline void FillRandom(int* arr, std::size_t size) {
		srand(time(nullptr));
		for (std::size_t index = 0; index < size; index++) {
			arr[index] = rand() % size + 1;
		}
	}
	inline void Copy(int* d_arr, int* s_arr, std::size_t size) {
		for (std::size_t index = 0; index < size; index++) {
			d_arr[index] = s_arr[index];
		}
	}
}

inline int* CreateArray(std::size_t size, char type, int* array) {
	int* arr = new int[size];
	switch (type) {
	case '1': ArrayDetails::FillRandom(arr, size); break;
	case '2': for (std::size_t index = 0; index < size; index++) {
		arr[index] = index + 1;
	} break;
	case '3': for (int index = size - 1; index >= 0; index--) {
		arr[index] = index + 1;
	} break;
	case '4': ArrayDetails::Copy(arr, array, size);
	}
	return arr;
}
