#include <iostream>
#include <chrono>

#include "Sortings.h"
#include "ArrayUtils.h"

int main() {
	bool finish = false;
	while (!finish) {
		std::size_t array_size = 0;
		char choice = '0';
		std::cout << "Enter the size of array: ";
		std::cin >> array_size;
		std::cout << "Choose the data type you want the array to be filled with:\n1)Random;\n2)Ascending order;\n3)Descending order;\nEnter 1, 2 or 3 to make a choice: ";
		std::cin >> choice;
		int* array0 = CreateArray(array_size, choice, nullptr);
		int* array1 = CreateArray(array_size, '4', array0);
		int* array2 = CreateArray(array_size, '4', array0);
		int* array3 = CreateArray(array_size, '4', array0);
		std::cout << "Bubble Sort\n";
		BubbleSort(array0, array_size);
		std::cout << "Insertion Sort\n";
		InsertionSort(array1, array_size);
		std::cout << "Merge Sort\n";
		MergeSort(array2, array_size);
		std::cout << "Quick Sort\n";
		QuickSort(array2, array_size);
		char answer = 'a';
		std::cout << "Repeat? (y/n)"; std::cin >> answer;
		if (answer == 'n') finish = true;
		delete[] array0; delete[] array1; delete[] array2; delete[] array3;
	}
	return 0;
}