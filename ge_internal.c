#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ge_math.h"
#include "common.h"

#if USE_MPI
#include <mpi.h>
#endif 

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
 * ge_dtect_internal_tthreshold - detect the error using 
 *                                tthreshold
 * @ratio, change ratio of current timestep to prev timestep 
 *
 * the method is similart threshold, but check on the mean 
 * value of the whole timestep 
 */
int ge_detect_internal_tthreshold(vec_double_t ratio)
{
	int i;
	double mean = ge_mean(ratio.array, 1, ratio.size);
#ifdef DEBUG_INTERNAL
	fprintf(stderr, "method: %s, mean ratio: %f, thresh: %f\n",
		__func__, mean, threshold);
#endif
	if (fabs(mean) > threshold)
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
int ge_detect_internal_mean(vec_double_t ratio)
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
 * ge_detect_internal_linear_fit - detect faults using linear fit method 
 * @ratio, change ratio of current timestep to prev timestep 
 *
 * The method treat the change ratio of successive timesteps as a linear 
 * function, using the history data to estimate the linear parameters for
 * each location
 */
int ge_detect_internal_linear_fit(vec_double_t ratio)
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

/**
 * ge_detect_internal_tmean - detect faults using tmean method 
 * @ratio, change ratio of current timestep to prev timestep 
 *
 * The method is similar to mean method, but target for the mean value 
 * of each time step, not for each location
 */
int ge_detect_internal_tmean(vec_double_t ratio)
{
	int i, steps, elems, res = GE_NORMAL, full;
	double step_mean, hist_mean, stdv, *buffer;

	buffer = history.data;
	steps = history.steps;
	elems = history.dim;

	// timestep based method, history just has one data, mean change ratio
	if (elems != 1) {
		log_err("dimension size not match with tmean method\n");
		exit(EXIT_FAILURE);
	}
	// mean value of current timestep
	step_mean = ge_mean(ratio.array, 1, ratio.size);

	full = ge_buffer_status();
	if (full) {
		// mean and stdv values of means of history steps
		hist_mean = ge_mean(buffer, elems, steps);
		stdv = ge_stdv(buffer, elems, steps);
		if (fabs(step_mean - hist_mean) > threshold * stdv)
			res = GE_FAULT;
#ifdef DEBUG_INTERNAL
		fprintf(stderr, "method: %s, hist_mean: %f, stdv: %f,"
			"mean_ratio: %f thresh: %f, diff: %f, res: %d\n",
			__func__, hist_mean, stdv, step_mean, threshold,
			fabs(step_mean - hist_mean), res);
#endif
	}

	ge_buffer_append(&step_mean, elems);
	return res;
}

/**
 * ge_detect_internal_tmean_linear - detect faults using 
 *                                  tmean_linear method 
 * @ratio, change ratio of current timestep to prev timestep 
 *
 * The method is similar to linear_Fit method, but 
 * target for the mean value of each time step, 
 * not for each location
 */
int ge_detect_internal_tmean_linear(vec_double_t ratio, int step)
{
	int i, steps, elems, full, res = GE_NORMAL;
	double step_mean, hist_mean, a, b, predict, stdv, *x, *buffer;

	buffer = history.data;
	steps = history.steps;
	elems = history.dim;

	// timestep based method, history just has one data, mean change ratio
	if (elems != 1) {
		log_err("dimension size not match with tmean_linear method\n");
		exit(EXIT_FAILURE);
	}
	// calc mean value of current time step
	step_mean = ge_mean(ratio.array, 1, ratio.size);

#if USE_MPI
	MPI_Allreduce(&step_mean, &step_mean, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	step_mean = step_mean / ge_comm_size;
#endif

	full = ge_buffer_status();
	if (full && step % ge_freq == 0) {
		stdv = ge_stdv(buffer, elems, steps);

		x = (double *)malloc(steps * sizeof(double));
		if (x == NULL) {
			log_err("linear_fit, mem allocation error");
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < steps; i++)
			x[i] = i;

		ge_lstsq(x, buffer, &a, &b, steps, elems);
		predict = a * (steps + 1) + b;
#ifdef DEBUG_INTERNAL
		fprintf(stderr, "method: %s, predict: %f, stdv: %f, \
                mean_ratio: %f thresh: %f\n", __func__, predict, stdv, step_mean, threshold);
#endif
		if (fabs(predict - step_mean) > threshold * stdv) {
			res = GE_FAULT;
		}
	}

	ge_buffer_append(&step_mean, elems);
	return res;
}
