#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ge_list.h"
#include "ge_math.h"
#include "common.h"


static int method = 0;
static double threshold = 0.0;
static int window_size = 0; 

// recording last time step data for calcing change ratio
vec_double_t last_time_step; 


#define log_err(str) fprintf(stderr, "%s", str)

/* API definition */

/* init the library. */
void ge_detect_init(int dmethod, int win_size, double thresh)
{
	switch (dmethod){
		case THRESHOLD_METHOD:
			method = THRESHOLD_METHOD;
			threshold = thresh;
			break;
		case LINEAR_FIT:
			method = LINEAR_FIT;
			window_size = win_size;
			ge_list_init(win_size);
			break;
		default:
			log_err("undefined method\n");
	}
}

/* detect faults with threshold method, 
   input is the change ratio
 */
int ge_detect_internal_threshold(vec_double_t ratio)
{
	int i;
	for ( i = 0; i < ratio.size; i ++)
		if (ratio.array[i] > threshold)
			return FAULT;
	return NORMAL;

}


/* detect faults using linear fit method */
int ge_detect_internal_linear_fit(vec_double_t ratio)
{
	double *a = NULL, *b = NULL, *predit = NULL;
	int i, elems = 0, res=NORMAL;

	// check whether history buf is full, if not just append current data
	int full = ge_list_get_status();

	if (!full) {
		ge_list_append(ratio.array, ratio.size);
		return NORMAL;
	}

	predit = (double *) malloc (sizeof(double) * ratio.size);
	if (predit == NULL) {
		log_err("allocate memory for prediction array error, exit the program\n");
		exit(-1);
	}

	ge_lstsq(&a, &b, &elems);

	// check whether a fault detected
	for ( i = 0; i < ratio.size; i++) {
		predit[i] = a[i] * (window_size + 1) + b[i] + 0.005;
		if (ratio.array[i] > predit[i]) {
			//log_err("fault detected\n");
			//free(predit);
			//exit(-1);
			res = FAULT;
			break;
			
		}
	}
	ge_list_append(ratio.array, ratio.size);
	free(predit);

	return res;
}


/* verify the current state. first calc the 
   change ratio, then evaluate the change ratio
   if no fault detected, append the change ratio 
   to history buf. otherwise quit the application
   it should be an MPI version for sync quit 
 */
int ge_detect_verify(double *buf, int buf_size) 
{
	int i, result;
	vec_double_t ratio;
	double *tmp = (double *) malloc(sizeof(double) * buf_size);
	if ( tmp == NULL ) {
		log_err("alloc memory error\n");
		exit(-1);
	}
	
	for ( i = 0; i < buf_size; i++) {
		tmp[i] = buf[i];
	}

	// the first step, no record for calcing the change ratio
	if (last_time_step.array == NULL) {
		last_time_step.array = tmp;
		return NORMAL;
	}

	ratio.array = (double *) malloc(sizeof(double) * buf_size);
	if (ratio.array == NULL ) {
		log_err("alloc memory error\n");
		exit(-1);
	}
	
	ratio.size = buf_size;

	// calc change ratio
	for ( i = 0; i < buf_size; i++) {
		if (last_time_step.array[i] == 0)
			last_time_step.array[i] = 1.0;
		ratio.array[i] = buf[i]/last_time_step.array[i] - 1;
	}

	// update the last time step data
	if (last_time_step.array != NULL)
		free(last_time_step.array);
	last_time_step.array = tmp;

	// call related method
	switch (method) {
		case THRESHOLD_METHOD:
			result = ge_detect_internal_threshold(ratio);
			break;
		case LINEAR_FIT:
			result = ge_detect_internal_linear_fit(ratio);
			break;
		default:
			log_err("method not defined\n");
	}

	//free up the ratio array
	free(ratio.array);

	return result;
}


void ge_detect_finish()
{
	if (last_time_step.array)
		free(last_time_step.array);
	ge_list_clean();
}
