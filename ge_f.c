
#include "ge.h"

void ge_detect_init_(int *method, int *win_size, double * thresh)
{
	ge_detect_init(*method, *win_size, *thresh);
}

int ge_detect_verify_(double buf[], int *buf_size) 
{
	int result;
	result = ge_detect_verify(buf, *buf_size);
	return result;
}

void ge_detect_finish_()
{
	ge_detect_finish();
}
