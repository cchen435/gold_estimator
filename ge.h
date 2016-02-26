#ifndef __GE_DETECT_H_
#define __GE_DETECT_H_

#include "common.h"

void ge_detect_init(dmethods method, int array_size, int win_size, double thresh, int freq);
int ge_detect_verify(double *buf, int buf_size, int step);
void ge_detect_finalize(void);

/* Fortran API */
void ge_detect_init_(dmethods *method, int *array_size, int *win_size, double *thresh, int *freq);
int ge_detect_verify_(double buf[], int *buf_size, int *step);
void ge_detect_finalize_();
#endif
