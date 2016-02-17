
#include<stdio.h>
#include<gsl/gsl_fit.h>
#include<gsl/gsl_statistics_double.h>

void ge_lstsq(double x[], double y[], double *a, double *b, int steps, int elems)
{
    int i;
    double tmp;

    gsl_fit_linear(x, 1, buffer, elems, steps, b, a,
            &tmp, &tmp, &tmp, &tmp);

}

double ge_mean(double data[], int stride, int size) 
{
   return gsl_stats_mean(data, stride, size);
}

double ge_stdv(double data[], int stride, int size)
{
    return gsl_stats_sd(data, stride, size);
}
