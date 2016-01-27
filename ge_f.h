#ifndef __GE_DETECT_H_
#define __GE_DETECT_H_

void ge_detect_init(int method, int win_size, double thresh);
int ge_detect_verify(double *buf, int buf_size);
void ge_detect_finish(void);

/* Fortran API */
void ge_detect_init_(int *method, int *win_size, double *thresh);
int ge_detect_verify_(double buf[], int *buf_size);
void ge_detect_finish_();
#endif
