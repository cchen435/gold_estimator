
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

		fprintf(stdout, "mean = %f, stdv = %f\n", mean, stdv);
	}

	printf("\n\n");

	/* test ge_mean and ge_stdv along with 2n dimension */
	for (i = 0; i < 5; i++) {
		mean = ge_mean(&b[i], 5, 4);
		stdv = ge_stdv(&b[i], 5, 4);

		fprintf(stdout, "mean = %f, stdv = %f\n", mean, stdv);
	}

	printf("b with stride 5, offset 1:\n");
	for (i = 0; i < 20; i += 5)
		printf("%f ", b[i + 1]);
	printf("\n");

	/* test ge_lstsq */
	ge_lstsq(x, &b[1], &a, &c, 5, 4);
	printf("a=%f, c=%f\n", a, c);
	printf("\n\n");
	printf("b with stride 5, offset 4:\n");
	for (i = 0; i < 20; i += 5)
		printf("%f ", b[i + 4]);
	printf("\n");

	ge_lstsq(x, &b[4], &a, &c, 5, 4);
	printf("a=%f, c=%f\n", a, c);
	printf("\n\n");

    int x2[3] = {0,1,2, 3, 4};
    int x3[3] = {1,2,3};
    //double b1[5] = {0.106007, 0.081270, 0.057180, 0.041822, 0.027249};
    double b1[15] = { 0.008972 , 1, 2, 
                      0.007118 , 3, 4, 
                      0.005093 , 5, 6, 
                      0.001670 , 7, 8, 
                      0.002885 , 9, 10};
    double sumsq = ge_lstsq(x2, b1, &a, &c, 3, 5);
	printf("a=%f, c=%f, sumsq=%f\n", a, c, sumsq);
    printf("b1[0]: %f, b1[1]: %f, b1[2]: %f, b1[3]: %f, b1[4]: %f, b1[5]: %f\n", c, a+c, 2*a+c ,a*3+c, a*4+c, a*5+c);

	return 0;

}
