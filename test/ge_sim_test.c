#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "ge.h"

int main(int argc, char *argv)
{
	double temperature[10];
	int curr_step = 0;
	int total_time_steps = 1000;
	int i, result;
	double ratio = 1.02;

	double max = 0.0;
	int window_size = 5;

	for (i = 0; i < 10; i++)
		temperature[i] = 12.0;

	ge_detect_init(MEAN_LINEAR, 10, window_size, 1.0);

	for (curr_step = 0; curr_step < total_time_steps; curr_step++) {
		fprintf(stderr, "\n\nstep : %d\n", curr_step + 1);
		srand(time(NULL));
		max = 0.0;
		for (i = 0; i < 10; i++) {
			double random_change = rand() % 10 / 1000.0 + ratio
			    if ((curr_step + 1) % 20 == 0)
				random_change += 0.06;
			temperature[i] *= random_change;
			if (max < random_change) {
				max = random_change;
			}
		}
		result = ge_detect_verify(temperature, 10);
		if (result == FAULT)
			fprintf(stderr,
				"a fault detected with max change ratio: %f\%\n",
				(max - 1.0) * 100);
	}

	ge_detect_finish();
}
