#ifndef __GE_MATH_H
#define __GE_MATH_H

#include "ge_list.h"

extern void ge_lstsq(double **a, double **b, int *elems);
extern double ge_mean(double data[], int stride, int size);
extern double ge_stdv(double data[], int stride, int size);
#endif
