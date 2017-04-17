
#include<stdio.h>
#include "ge_math.h"

/**
 * this file tests the ge_math implementation
 */

int main()
{
	double array[4][5];
	double b[20];
	double mean, stdv;
	int i, j;

	double x[4] = { 1, 2, 3, 4 };
	double a, c;

	/* initialzie the array */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 5; j++) {
			array[i][j] = i + j;
			b[i * 5 + j] = i + j;
			printf("%d ", i + j);
		}
		printf("\n");
	}

	/* test ge_mean and ge_stdv along with first dimension */
	for (i = 0; i < 4; i++) {
		mean = ge_mean(array[i], 1, 5);
		stdv = ge_stdv(array[i], 1, 5);

		fprintf(stdout, "mean = %.12f, stdv = %.12f\n", mean, stdv);
	}

	printf("\n\n");

	/* test ge_mean and ge_stdv along with 2n dimension */
	for (i = 0; i < 5; i++) {
		mean = ge_mean(&b[i], 5, 4);
		stdv = ge_stdv(&b[i], 5, 4);

		fprintf(stdout, "mean = %.12f, stdv = %.12f\n", mean, stdv);
	}

	printf("b with stride 5, offset 1:\n");
	for (i = 0; i < 20; i += 5)
		printf("%.12f ", b[i + 1]);
	printf("\n");

	/* test ge_lstsq */
	ge_lstsq(x, &b[1], &a, &c, 5, 4);
	printf("a=%.12f, c=%.12f\n", a, c);
	printf("\n\n");
	printf("b with stride 5, offset 4:\n");
	for (i = 0; i < 20; i += 5)
		printf("%.12f ", b[i + 4]);
	printf("\n");

	ge_lstsq(x, &b[4], &a, &c, 5, 4);
	printf("a=%.12f, c=%.12f\n", a, c);
	printf("\n\n");

    double x2[3] = {0,1,2};
    int x3[3] = {1,2,3};
    double b1[9] = { 0.000000000000, 1, 2, 
                      -0.000001615434, 3, 4, 
                      -0.000003671474, 5, 6};
    double b2[3] = { 0.000000000000, -0.000001615434, -0.000003671474};
    double sumsq = ge_lstsq(x2, b1, &a, &c, 3, 3);
    printf("b1[0]: %.12f, b1[1]: %.12f, b1[2]: %.12f\n", b2[0], b2[1], b2[2]);
	printf("a=%.12f, c=%.12f, sumsq=%.12f\n", a, c, sumsq);
    printf("b1[0]: %.12f, b1[1]: %.12f, b1[2]: %.12f, b1[3]: %.12f, b1[4]: %.12f, b1[5]: %.12f\n", c, a+c, 2*a+c ,a*3+c, a*4+c, a*5+c);
    sumsq = ge_lstsq(x2, b2, &a, &c, 1, 3);
	printf("a=%.12f, c=%.12f, sumsq=%.12f\n", a, c, sumsq);
    printf("b1[0]: %.12f, b1[1]: %.12f, b1[2]: %.12f, b1[3]: %.12f, b1[4]: %.12f, b1[5]: %.12f\n", c, a+c, 2*a+c ,a*3+c, a*4+c, a*5+c);

	return 0;

}
