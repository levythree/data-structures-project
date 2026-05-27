#include "compare.h"

int compareInt(void* data1, void* data2) {
    return *(int*) data1 - *(int*) data2;
}
