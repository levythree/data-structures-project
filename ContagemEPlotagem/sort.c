#include "sort.h"

void swap(int* a, int* b) {
    int temporary = *a;
    *a = *b;
    *b = temporary;
}

void quickSort(int* array, int size) {
    if (size <= 1) return;
    else {
        int pivot = array[size / 2], a = 0, b = size - 1;

        while (a <= b) {
            while (array[a] < pivot) a++;
            while (array[b] > pivot) b--;

            if (a <= b) swap(&array[a++], &array[b--]);
        }

        quickSort(array, b + 1);
        quickSort(array + a, size - a);
    }
}
