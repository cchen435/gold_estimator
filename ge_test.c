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

	for (curr_step = 0; curr_step < total_time_steps; curr_step ++) {
		fprintf(stderr, "step : %d\n", curr_step+1);
		srand(time(NULL));
		for ( i = 0; i < 10; i++) {
			double random_change = 1.0 + rand() % 33 / 1000.0;
			temperature[i] *= random_change;
			if (max < random_change) {
				max = random_change;
			}
		}
		result = ge_detect_verify(temperature, 10);
		if (result == FAULT)
			fprintf(stderr, \
				"a fault detected with max change ratio: %f\n", \
				max - 1.0);
	}

	ge_detect_finish();
}
