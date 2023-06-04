#pragma once

#include <cstddef>
#include <random>

enum class Choice { random, asc_order, desc_order, copy };

namespace ArrayDetails {
	inline void FillRandom(int* arr, std::size_t size) {
		srand(time(nullptr));
		for (std::size_t index = 0; index < size; index++) {
			arr[index] = rand() % size + 1;
		}
	}
	inline void Copy(int* d_arr, const int* s_arr, std::size_t size) {
		for (std::size_t index = 0; index < size; index++) {
			d_arr[index] = s_arr[index];
		}
	}
}

inline std::unique_ptr<int[]> CreateArray(std::size_t size, Choice type, const int* array_to_copy = nullptr) {
	std::unique_ptr<int[]> arr = std::make_unique<int[]>(size);
	switch (type) {
	case Choice::random: ArrayDetails::FillRandom(arr.get(), size); break;
	case Choice::asc_order: for (std::size_t index = 0; index < size; index++) {
		arr[index] = index + 1;
	} break;
	case Choice::desc_order: for (int index1 = size - 1, index2 = 0; index1 >= 0; index1--) {
		arr[index2] = index1 + 1;
	} break;
	case Choice::copy: ArrayDetails::Copy(arr.get(), array_to_copy, size);
	}	return arr;
}

