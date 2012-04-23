#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

typedef union val_t {
	void *pointer;
	uint64_t bytes;
	double d;
	int i;
} val_t;



