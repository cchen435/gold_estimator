#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "ge.h"

int main (int argc, char *argv) 
{
	double temperature[10];
	int curr_step = 0;
	int total_time_steps = 1000;
	int i, result;

	double max = 0.0;

	double threshold = 0.03;

	for (i = 0; i < 10; i++)
		temperature[i] = 12.0;

	ge_detect_init (THRESHOLD_METHOD, 0, threshold);

	// iterate over time steps
	for (curr_step = 0; curr_step < total_time_steps; curr_step ++) {
		fprintf(stderr, "step : %d\n", curr_step+1);
		max = 0.0;
		// simulate the calculation for temperature
		for ( i = 0; i < 10; i++) {
			int randnum = rand() % 33;
			printf("%d ", randnum);
			double random_change = 1.0 + randnum / 1000.0;
			temperature[i] *= random_change;
			if (max < random_change) {
				max = random_change;
			}
			printf("\n");
		}

		// detect the faults
		result = ge_detect_verify(temperature, 10);
		if (result == FAULT)
			fprintf(stderr, \
				"a fault detected with max change ratio: %f\n", \
				max - 1.0);
		printf("\n----\n");
	}

	ge_detect_finish();
}
