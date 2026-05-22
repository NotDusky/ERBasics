#ifndef ER_ESSENTIALS_H
#define ER_ESSENTIALS_H

#include "Arduino.h"
#include "Bounce2.h"
#include "FastLED.h"

/**
RANDOM_NONE: Do not randomize.
RANDOM: Randomly selects values from the given set.
RANDOM_NO_REPEAT: Randomly selects values but will not repeat a value.
RANDOM_BD: Randomly selects values from the given set. 
-> Will only select one of each value from the set until it runs out of unique values.
RANDOM_BD_NO_REPEAT: RANDOM_BD without repeating values.
*/
enum RANDOM_MODE {
	RANDOM_NONE,
	RANDOM_NORMAL,
	RANDOM_NO_REPEAT,
	RANDOM_BD,
	RANDOM_BD_NO_REPEAT
};

namespace ERBasics {
	/*
	Randomises all elements in the given array.
	*/
	template<typename T, size_t size>
	void randomize_array(T(&arr)[size]) {
		// Shuffle the elements of the array.
		for (size_t i = size; i > 0; i--) {
			size_t j = random(0, i);
			T temp = arr[i - 1];
			arr[i - 1] = arr[j];
			arr[j] = temp;
		}
	}
	/*
	Randomises elements in the given array from range_start to range_end.
	*/
	template<typename T, size_t size>
	void randomize_subarray(T(&arr)[size], size_t range_start = 0, size_t range_end = size) {
		// Shuffle the elements of the array.
		for (size_t i = min(range_end, size); i > max(range_start, 0); i--) {
			size_t j = random(range_start, i);
			T temp = arr[i - 1];
			arr[i - 1] = arr[j];
			arr[j] = temp;
		}
	}
	/*
	Randomises all elements in the given array while maintaining groups of the given size.
	e.g. randomize_array_sections([0, 1, 2, 3, 4, 5], 4) would randomise the subarrays
	[0, 1, 2, 3] and [4, 5].
	*/
	template<typename T, size_t size>
	void randomize_array_sections(T(&arr)[size], size_t section_size = size) {
		int num_sections = size / section_size;
		for (int i = 0; i < size; i += section_size) {
			size_t range_max = i + section_size;
			randomize_subarray(arr, i, range_max);
		}
	}

	/* 
	Fills an array with integers starting from 0 and incrementing by 1.
	e.g. [0, 1, 2, 3].
	*/
	template<size_t size>
	void gen_sequence_array(int(&arr)[size], size_t overflow = size) {
		for (size_t i = 0; i < size; i++) {
			arr[i] = i % overflow;
		}
	}
	/* 
	Fills an array with integers starting from 0 and incrementing by 1.
	e.g. [0, 1, 2, 3].
	*/
	template<size_t size>
	void gen_sequence_array(uint8_t(&arr)[size], size_t overflow = size) {
		for (size_t i = 0; i < size; i++) {
			arr[i] = i % overflow;
		}
	}

	/* 
	Compares the elements in two arrays of the same type
	and returns true if all values are equal.
	*/
	template<typename T, size_t sizeA, size_t sizeB>
	bool compare_arrays(const T(&arrA)[sizeA], const T(&arrB)[sizeB]) {
		for (int i = 0; i < min(sizeA, sizeB); i++) {
			if (arrA[i] != arrB[i]) {
				return false;
			}
		}
		return true;
	}

	template<typename T, size_t sizeA, size_t sizeB>
	void fill_array(T(&arrA)[sizeA], const T(&arrB)[sizeB], RANDOM_MODE mode) {
		switch (mode) {
			
		case (RANDOM_BD_NO_REPEAT): {
			int iter_nr = ceil((float)sizeA / sizeB);
			for (int i = 0; i < iter_nr; i++) {
				T temp_arr[sizeB] = {};
				for (int j = 0; j < sizeB; j++) { temp_arr[j] = arrB[j]; }
				int temp_size = sizeB;
				int n_nr = min(sizeA, min(sizeB, sizeA - i * sizeB));
				for (int j = 0; j < n_nr; j++) {
					int y = i * sizeB + j;
					int x = 0;
					do {
						x = random(0, temp_size);
					} while (temp_arr[x] == arrA[y - 1] && y > 0);
					arrA[y] = temp_arr[x];
					for (int k = x; k < temp_size - 1; k++) { temp_arr[k] = temp_arr[k + 1]; }
					temp_size--;
				}
			}
		} break;

		case (RANDOM_BD): {
			int iter_r = ceil((float)sizeA / sizeB);
			for (int i = 0; i < iter_r; i++) {
				T temp_arr[sizeB] = {};
				for (int a = 0; a < sizeB; a++) { temp_arr[a] = arrB[a]; }
				int temp_size = sizeB;
				int n = min(sizeA, min(sizeB, sizeA - i * sizeB));
				for (int j = 0; j < n; j++) {
					int x = random(0, temp_size);
					arrA[i * sizeB + j] = temp_arr[x];
					for (int k = x; k < temp_size - 1; k++) { temp_arr[k] = temp_arr[k + 1]; }
					temp_size--;
				}
			}
		} break;

		case (RANDOM_NO_REPEAT): {
			for (int i = 0; i < sizeA; i++) {
				int n = 0;
				do {
					n = random(0, sizeB);
				} while (arrB[n] == arrA[i - 1] && i > 0);
				arrA[i] = arrB[n];
			}
		} break;

		case (RANDOM_NORMAL): {
			for (int i = 0; i < sizeA; i++) {
				int n = random(0, sizeB);
				arrA[i] = arrB[n];
			}
		} break;

		case (RANDOM_NONE): {
			for (int i = 0; i < sizeA; i++)
				arrA[i] = arrB[i % sizeB];
		} break;
		}
	}
}

namespace ERTesting {
	template<typename T, size_t size>
	void print_array(HardwareSerial& serial, T(&arr)[size]) {
		serial.print("[");
		for (size_t i = 0; i < size - 1; i++) {
			serial.print(arr[i]);
			serial.print(",");
		}
		serial.print(arr[size - 1]);
		serial.println("]");
	}
}

#endif