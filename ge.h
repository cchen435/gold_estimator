#ifndef __GE_DETECT_H_
#define __GE_DETECT_H_

#include "common.h"
#if 0
void ge_detect_init(dmethods method, int array_size, \
                    int win_size, double thresh, int freq);
int ge_detect_verify(double *buf, int buf_size, int step);
void ge_detect_finalize(void);

/* Fortran API */
void ge_detect_init_(dmethods *method, int *array_size, \
                     int *win_size, double *thresh, int *freq);
int ge_detect_verify_(double buf[], int *buf_size, int *step);
void ge_detect_finalize_();
#endif
/* C API */
void GE_Init();
void GE_Protect(char *var_name, void *var, int data_type, int size,
                double threshold, int window, int method, int use_chg_ratio,
                int granularity);
void GE_Protect_F(char *varname, int data_type, int size, double threshold,
                  int window, int method, int use_chg_ratio, int granularity);
void GE_Snapshot();
void GE_PrintResult();
void GE_Finalize();

/* Fortran API */
void GE_Init_();
void GE_PrintResult_();
void GE_Finalize_();

#endif
