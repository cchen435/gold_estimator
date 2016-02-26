
#include<stdio.h>
#include<gsl/gsl_fit.h>
#include<gsl/gsl_statistics_double.h>

int main()
{
	double array[4][5];
	double b[20];
	double mean, variance, stdv;
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 5; j++) {
			array[i][j] = i + j;
			b[i * 5 + j] = i + j;
			printf("%d ", i + j);
		}
		printf("\n");
	}

	for (i = 0; i < 4; i++) {
		mean = gsl_stats_mean(array[i], 1, 5);
		variance = gsl_stats_variance(array[i], 1, 5);
		stdv = gsl_stats_sd(array[i], 1, 5);

		fprintf(stdout, "mean = %f, var = %f, stdv = %f\n",
			mean, variance, stdv);
	}

	printf("\n\n");

	for (i = 0; i < 5; i++) {
		mean = gsl_stats_mean(&b[i], 5, 4);
		variance = gsl_stats_variance(&b[i], 5, 4);
		stdv = gsl_stats_sd(&b[i], 5, 4);

		fprintf(stdout, "mean = %f, var = %f, stdv = %f\n",
			mean, variance, stdv);
	}

	printf("\n\n");

	for (i = 0; i < 5; i++) {
		mean = gsl_stats_mean(&array[0][i], 5, 4);
		variance = gsl_stats_variance(&array[0][i], 5, 4);
		stdv = gsl_stats_sd(&array[0][i], 5, 4);

		fprintf(stdout, "mean = %f, var = %f, stdv = %f\n",
			mean, variance, stdv);
	}

	return 0;

}
