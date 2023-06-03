#include "Sortings.h"

#include <iostream>
#include <chrono>

struct Metrics {
	std::chrono::milliseconds sec_time;
	std::size_t comparison_number = 0;
	std::size_t insertion_number = 0;
};

static bool IsSorted(int* arr, std::size_t size) {
	std::size_t index = 1;
	while (index < size - 1) {
		if (arr[index] < arr[index - 1]) return false;
		index++;
	}
	return true;
}

static void PrintMetrics(Metrics m, int* arr, std::size_t size) {
	std::cout << "Sorting duration in milliseconds: " << m.sec_time.count() << " msec;\n"
		<< "Data comparisons: " << m.comparison_number << ";\n"
		<< "Data insertions: " << m.insertion_number << ";\n"
		<< "Is array fully sorted: ";
	if (IsSorted(arr, size)) std::cout << "yes\n";
	else std::cout << "no\n";
}

static void MSMerge(int* arr, int* temp_arr, std::size_t s_index, std::size_t m_index, std::size_t e_index, Metrics& ms_m) {
	std::size_t temp_s_i = s_index;
	std::size_t temp_m_i = m_index;
	std::size_t index = 0;
	while (temp_s_i < m_index && temp_m_i < e_index) {
		ms_m.comparison_number++;
		if (arr[temp_s_i] <= arr[temp_m_i]) temp_arr[index++] = arr[temp_s_i++];
		else temp_arr[index++] = arr[temp_m_i++];
	}
	if (temp_s_i == m_index) {
		while (temp_m_i < e_index) {
			temp_arr[index++] = arr[temp_m_i++];
		}
	}
	else {
		while (temp_s_i < m_index) {
			temp_arr[index++] = arr[temp_s_i++];
		}
	}
	for (std::size_t i = 0; i < index; i++) {
		ms_m.insertion_number++;
		arr[s_index + i] = temp_arr[i];
	}
}

static void MSSplit(int* arr, int* temp_arr, std::size_t s_index, std::size_t e_index, Metrics& ms_m) {
	if (s_index == e_index || s_index == e_index - 1) return;
	std::size_t m_index = (e_index + s_index) / 2;
	MSSplit(arr, temp_arr, s_index, m_index, ms_m);
	MSSplit(arr, temp_arr, m_index, e_index, ms_m);
	MSMerge(arr, temp_arr, s_index, m_index, e_index, ms_m);
}

static std::size_t QSDevide(int* arr, std::size_t s_index, std::size_t e_index, Metrics& qs_m) {
	std::size_t pivot = arr[(s_index + e_index) / 2];
	std::size_t temp_s_i = s_index;
	std::size_t temp_e_i = e_index;
	while (true) {
		while (arr[temp_s_i] < pivot) { qs_m.comparison_number++; temp_s_i += 1; }
		while (arr[temp_e_i] > pivot) { qs_m.comparison_number++; temp_e_i -= 1; }
		if (temp_s_i >= temp_e_i) return temp_e_i;
		std::swap(arr[temp_s_i++], arr[temp_e_i--]);
		qs_m.insertion_number += 2;
	}
}

static void QSSort(int* arr, std::size_t s_index, std::size_t e_index, Metrics& qs_m) {
	if (s_index < e_index) {
		std::size_t p_index = QSDevide(arr, s_index, e_index, qs_m);
		QSSort(arr, s_index, p_index, qs_m);
		QSSort(arr, p_index + 1, e_index, qs_m);
	}
}

void BubbleSort(int* arr, std::size_t size) {
	using namespace std::chrono;
	Metrics bs_m;
	std::size_t to_compare = size;
	auto start = high_resolution_clock::now();
	while (to_compare != 1) {
		for (std::size_t index = 1; index < to_compare; index++) {
			if (arr[index - 1] > arr[index]) {
				std::swap(arr[index - 1], arr[index]);
				bs_m.insertion_number += 2;
			}
			bs_m.comparison_number++;
		}
		to_compare--;
	}
	auto stop = high_resolution_clock::now();
	bs_m.sec_time = duration_cast<milliseconds>(stop - start);
	PrintMetrics(bs_m, arr, size);
}

void InsertionSort(int* arr, std::size_t size) {
	using namespace std::chrono;
	Metrics is_m;
	std::size_t index1 = 1;
	auto start = high_resolution_clock::now();
	while (index1 < size) {
		std::size_t index2 = index1;
		while (index2 > 0 && arr[index2 - 1] > arr[index2]) {
			std::swap(arr[index2], arr[index2 - 1]);
			is_m.comparison_number++;
			is_m.insertion_number += 2;
			index2 -= 1;
		}
		index1 += 1;
	}
	auto stop = high_resolution_clock::now();
	is_m.sec_time = duration_cast<milliseconds>(stop - start);
	PrintMetrics(is_m, arr, size);
}

void MergeSort(int* arr, std::size_t size) {
	using namespace std::chrono;
	Metrics ms_m;
	std::size_t s_index = 0, e_index = size - 1;
	int* temp_arr = new int[size];
	auto start = high_resolution_clock::now();
	MSSplit(arr, temp_arr, s_index, e_index, ms_m);
	auto stop = high_resolution_clock::now();
	ms_m.sec_time = duration_cast<milliseconds>(stop - start);
	PrintMetrics(ms_m, temp_arr, size);
}

void QuickSort(int* arr, std::size_t size) {
	using namespace std::chrono;
	Metrics qs_m;
	std::size_t s_index = 0, e_index = size - 1;
	auto start = high_resolution_clock::now();
	QSSort(arr, s_index, e_index, qs_m);
	auto stop = high_resolution_clock::now();
	qs_m.sec_time = duration_cast<milliseconds>(stop - start);
	PrintMetrics(qs_m, arr, size);
}
