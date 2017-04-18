#ifndef __GE_DETECT_H_
#define __GE_DETECT_H_

#include "common.h"
/* C API */
void GE_Init();
void GE_Protect(char *var_name, void *var, int data_type, int size,
                double threshold, int window, int method, int use_chg_ratio,
                int granularity);
void GE_Protect_F(char *varname, int data_type, int size, double threshold,
                  int window, int method, int use_chg_ratio, int granularity);
void GE_Snapshot();
void GE_Snapshot_1var_F(GE_dataset *pp, void *var);
void GE_PrintResult();
void GE_Finalize();

/* Fortran API */
void GE_Init_();
void GE_PrintResult_();
void GE_Finalize_();

#endif
