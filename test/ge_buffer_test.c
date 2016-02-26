#include <stdio.h>
#include "common.h"
#include "ge_buffer.h"

extern struct _hist_buffer history;
int main(int argc, char **argv)
{
	int i, j, k, l;
	double data[4] = { 1, 1.5, 2, 2.5 };
	ge_buffer_init(2, 4);

	int steps, dim;
	double *h;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 4; j++)
			data[j] += 1;

		ge_buffer_append(data, 4);

		if (ge_buffer_status()) {
			/*
			   dim = ge_buffer_dim();
			   steps = ge_buffer_steps();
			   h = ge_buffer_data();
			 */
			dim = history.dim;
			steps = history.steps;
			h = history.data;
			printf("buff tst data in buff:\n");
			for (k = 0; k < steps; k++) {
				for (l = 0; l < dim; l++)
					printf("%f ", h[k * dim + l]);
				printf("\n");
			}
		}

		printf("\n\n");
	}

	ge_buffer_clean();
}
