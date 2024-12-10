#include <stdio.h>

typedef __typeof__(int (*)(int, int)) AddFunc;

int func(AddFunc add, int x, int y) {
    return add(x, y);
}