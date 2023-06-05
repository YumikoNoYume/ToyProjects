#include <iostream>
#include <chrono>

#include "Sortings.h"
#include "ArrayUtils.h"

int main() {
	bool finish = false;
	while (!finish) {
		ArrayTypeChoice type;
		std::size_t array_size = 0;
		char choice = '0';
		std::cout << "Enter the size of array: ";
		std::cin >> array_size;
		std::cout << "Choose the data type you want the array to be filled with:\n1)Random;\n2)Ascending order;\n3)Descending order;\nEnter 1, 2 or 3 to make a choice: ";
		std::cin >> choice;
		switch (choice) {
		case '1': type = ArrayTypeChoice::random; break;
		case '2': type = ArrayTypeChoice::asc_order; break;
		case '3': type = ArrayTypeChoice::desc_order;
		}
		std::unique_ptr<int[]> array0 = CreateArray(array_size, type);
		std::unique_ptr<int[]> array1 = CreateArray(array_size, ArrayTypeChoice::copy, array0.get());
		std::unique_ptr<int[]> array2 = CreateArray(array_size, ArrayTypeChoice::copy, array0.get());
		std::unique_ptr<int[]> array3 = CreateArray(array_size, ArrayTypeChoice::copy, array0.get());
		std::cout << "Bubble Sort\n";
		BubbleSort(array0.get(), array_size);
		std::cout << "Insertion Sort\n";
		InsertionSort(array1.get(), array_size);
		std::cout << "Merge Sort\n";
		MergeSort(array2.get(), array_size);
		std::cout << "Quick Sort\n";
		QuickSort(array3.get(), array_size);
		char answer = 'a';
		std::cout << "Repeat? (y/n) "; std::cin >> answer;
		if (answer == 'n') finish = true;
	}
	return 0;
}