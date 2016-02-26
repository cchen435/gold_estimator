
#include "ge.h"

void ge_detect_init_(dmethods * method, int *array_size, int *win_size,
		     double *thresh, int *freq)
{
	ge_detect_init(*method, *array_size, *win_size, *thresh, *freq);
}

int ge_detect_verify_(double buf[], int *buf_size, int *step)
{
	int result;
	result = ge_detect_verify(buf, *buf_size, *step);
	return result;
}

void ge_detect_finalize_()
{
	ge_detect_finalize();
}
