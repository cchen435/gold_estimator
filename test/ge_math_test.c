
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
    ge_lstsq(x, &b[1], &a, &c, 4, 5);
    printf("a=%f, c=%f\n", a, c);
    printf("\n\n");
    ge_lstsq(x, &b[4], &a, &c, 4, 5);
    printf("a=%f, c=%f\n", a, c);
    printf("\n\n");

    return 0;

}
