#ifndef __GE_MATH_H
#define __GE_MATH_H

extern double ge_lstsq(double x[], double y[], double *a, double *b, int stride, int steps);
extern double ge_mean(double data[], int stride, int size);
extern double ge_range(double data[], int stride, int size);
extern double ge_stdv(double data[], int stride, int size);
#endif
