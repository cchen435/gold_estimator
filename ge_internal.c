#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ge_math.h"
#include "common.h"

#if USE_MPI
#include <mpi.h>
#endif 

level_t level;

dmethods method = NONE;
double threshold = 0.0;
extern struct _hist_buffer history;
int ge_freq;

#if USE_MPI
int ge_rank;
int ge_comm_size;
#endif

/** GE Internal API definition*/

/**
 * ge_dtect_internal_threshold - detect the error using threshold
 * @ratio, change ratio of current timestep to prev timestep 
 */
int ge_detect_internal_threshold(vec_double_t ratio)
{
	int i;
	for (i = 0; i < ratio.size; i++)
		if (fabs(ratio.array[i]) > threshold)
			return GE_FAULT;
	return GE_NORMAL;
}

/**
 * ge_detect_internal_mean - using mean method to detect error
 * @ratio, change ratio of current timestep to prev timestep 
 * 
 * The method compares the current change ratio to the mean 
 * value of history change ratio for each loaction
 */
int ge_detect_internal_statistic(vec_double_t ratio)
{
	int i, j, steps, elems;
	static int res;
	double *buffer = history.data;

	int full = ge_buffer_status();

	res = GE_NORMAL;

	if (full) {
		steps = history.steps;
		elems = history.dim;

		for (i = 0; i < elems; i++) {
			double mean, stdv;
			stdv = ge_stdv(&buffer[i], elems, steps);
			mean = ge_mean(&buffer[i], elems, steps);
			if (fabs(ratio.array[i] - mean) > threshold * stdv) {
				res = GE_FAULT;
				break;
			}
		}
	} else {
		printf("mean: buffer is not full\n");
	}
	ge_buffer_append(ratio.array, ratio.size);
	return res;
}

/**
 * ge_detect_internal_linear - detect faults using linear fit method 
 * @ratio, change ratio of current timestep to prev timestep 
 *
 * The method treat the change ratio of successive timesteps as a linear 
 * function, using the history data to estimate the linear parameters for
 * each location
 */
int ge_detect_internal_linear(vec_double_t ratio)
{
	double *x, *buffer;
	double a, b, predict, stdv;
	int i, steps, elems, res = GE_NORMAL;

	// check whether history buf is full, if not just append current data
	int full = ge_buffer_status();
	if (full) {
		steps = history.steps;
		elems = history.dim;
		buffer = history.data;

		x = (double *)malloc(steps * sizeof(double));
		if (x == NULL) {
			log_err("linear_fit, mem allocation error");
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < steps; i++)
			x[i] = i;

		// check whether a fault detected
		for (i = 0; i < ratio.size; i++) {
			stdv = ge_stdv(&buffer[i], elems, steps);
			ge_lstsq(x, &buffer[i], &a, &b, steps, elems);
			predict = a * (steps + 1) + b;
			if (fabs(ratio.array[i] - predict) > threshold * stdv) {
				//log_err("fault detected\n");
				//free(predit);
				//exit(-1);
				res = GE_FAULT;
				break;
			}
		}
	} else {
		printf("linear: buffer is not full\n");
	}
	ge_buffer_append(ratio.array, ratio.size);
	return res;
}
