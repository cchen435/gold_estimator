
/* learn/test the least square method from gsl library */

#include<gsl/gsl_fit.h>
#include<stdio.h>

int main(int argc, char *argv)
{
	double x[] = {1, 2, 3, 4, 5, 6};
	double y[] = {3, 5, 7, 9, 11, 13};
	double a, b, cov00, cov01, cov11, sumsq;
	
	a = b = cov00 = cov01 = cov11 = sumsq = 0.0;
        gsl_fit_linear(x, 1, y, 1, 6, &b, &a, &cov00, &cov01, &cov11, &sumsq);

	printf("a = %f, b = %f\n", a, b);
	printf("cov = %f, %f, %f, sumsq = %f\n", cov00, cov01, cov11, sumsq);
	return 0;
}
