#include <stdio.h>

#include "comparisons.h"

int compareInt(void* a, void* b) {
    return *(int*) a - *(int*) b;
}
