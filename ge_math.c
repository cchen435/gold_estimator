
#include<stdio.h>
#include<gsl/gsl_fit.h>
#include<gsl/gsl_statistics_double.h>

double ge_lstsq(double x[], double y[], double *a, double *b, int stride, int steps)
{
	double tmp;
    double sumsq;
	gsl_fit_linear(x, 1, y, stride, steps, b, a, &tmp, &tmp, &tmp, &sumsq);
    return sumsq;
}


double ge_range(double data[], int stride, int size) {
    double min, max;
    gsl_stats_minmax(&min, &max, data, stride, size);
    return fabs(max-min);
}

double ge_mean(double data[], int stride, int size)
{
	return gsl_stats_mean(data, stride, size);
}

double ge_stdv(double data[], int stride, int size)
{
	return gsl_stats_sd(data, stride, size);
}
