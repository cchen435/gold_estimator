#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ge_list.h"
#include "ge_math.h"
#include "common.h"

static int method = 0;
static int window_size = 0;
static double threshold = 0.0;
extern struct _hist_buffer history;

/**
 * recording the data from last timestep
 * used for calcing change ratio
 */
vec_double_t last;

#define log_err(str) fprintf(stderr, "%s", str)

/** GE Internal API definition*/

/**
 * ge_dtect_internal_threshold - detect the error using threshold
 * @ratio, change ratio of current timestep to prev timestep 
 */
int ge_detect_internal_threshold(vec_double_t ratio)
{
    int i;
    for (i = 0; i < ratio.size; i++)
	if (abs(ratio.array[i]) > threshold)
	    return FAULT;
    return NORMAL;
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
    double mean = ge_mean(ratio.array, ratio.size);
    if (abs(mean) > threshold)
	    return FAULT;
    return NORMAL;
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
    int i, j, steps, elems, res=NORMAL;
    double mean, stdv;
    double *buffer = history.data;

    int full = ge_buffer_status();
    if ( full ) {
        steps = history.steps;
        elems = history.dim;

        for (i = 0; i < elems; i++) {
            sum = 0.0;
            mean = 0.0;
            stdv = ge_stdv(&buffer[i], elems, steps);
            mean = ge_mean(&buffer[i], elems, steps);
            if (abs(ratio.array[i] - mean) > threshold * stdv){
                res = FAULT;
                break;
            }
        }
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
    int i, steps, elems, res = NORMAL;

    // check whether history buf is full, if not just append current data
    int full = ge_buffer_status();
    if (full) {
        steps = history.steps;
        elems = history.dim;
        buffer = history.data;
        
        x = (double *) malloc(steps * sizeof(double));
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
            predict = a * (window_size + 1) + b + stdv;
            if (abs(ratio.array[i] - predict) > threshold) {
	        //log_err("fault detected\n");
	        //free(predit);
	        //exit(-1);
	        res = FAULT;
	        break;
            }
        }
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
    int i,steps, elems, res=NORMAL, full;
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
        if (abs(step_mean - hist_mean) > threshold * stdv)
            res = FAULT;
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
int ge_detect_internal_tmean_linear(vec_double_t ratio)
{
    int i,steps, elems, elems, full, res=NORMAL;
    double step_mean, hist_mean, a, b, predict, stdv, *buffer;
    
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

    full = ge_buffer_status();
    if (full) {
        stdv = ge_stdv(buffer, elems, steps);
        ge_lstsq(x, buffer, &a, &b, steps, elems);
        predict = a * (steps + 1) + b + stdv;
        if (abs(predict - step_mean) > threshold * stdv) {
            res = FAULT;
            break;
        }
    }
    
    ge_buffer_append(&step_mean, elems);
    return res;
}



/* GE API definition */
/**
 * ge_detect_init -- initialize the library
 * @dmethod: the detection method 
 * @win_size: window size of buffer
 * @thresh: threshold value used for threshold method
 *          or threshold to measure the difference between 
 *          and actual value 
 */
void ge_detect_init(int dmethod, int array_size, int win_size, double thresh)
{
    method = dmethod;
    threshold = thresh;

    switch (dmethod) {
	case THRESHOLD:
            break;
        case TTHREASHOLD:
	    break;
	case LINEAR:
	case MEAN:
            if (threshold < 1.0) {
                log_err("for linear, mean, tmean, mean_linear, "
                        "threhsold represnts number of stdvs, be larger then 1");
                exit(EXIT_FAILURE);
            }
	    window_size = win_size;
	    ge_buffer_init(win_size, array_size);
	    break;
	case TMEAN:
	case MEAN_LINEAR:
            if (threshold < 1.0) {
                log_err("for linear, mean, tmean, mean_linear,"
                        "threhsold represnts number of stdvs,"
                        "be larger then 1");
                exit(EXIT_FAILURE);
            }
	    window_size = win_size;
	    ge_buffer_init(win_size, 1);
	    break;
	default:
	    log_err("undefined method\n");
    }
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
    double *tmp = (double *)malloc(sizeof(double) * buf_size);
    if (tmp == NULL) {
	log_err("alloc memory error\n");
	exit(-1);
    }

    // process based on absolute value to avoid divide by zeros
    for (i = 0; i < buf_size; i++) {
	tmp[i] = abs(buf[i]);
    }

    // the first step, no record for calcing the change ratio
    if (last.array == NULL) {
	last.array = tmp;
	return NORMAL;
    }

    ratio.array = (double *)malloc(sizeof(double) * buf_size);
    if (ratio.array == NULL) {
	log_err("alloc memory error\n");
	exit(-1);
    }

    ratio.size = buf_size;

    // calc change ratio
    for (i = 0; i < buf_size; i++) {
	last.array[i] += 1.0;
	ratio.array[i] = tmp[i] / last.array[i] - 1;
    }

    // update the last time step data
    if (last.array != NULL)
	free(last.array);
    last.array = tmp;

    // call related method
    switch (method) {
	case THRESHOLD:
	    result = ge_detect_internal_threshold(ratio);
	    break;
	case TTHRESHOLD:
	    result = ge_detect_internal_tthreshold(ratio);
	    break;
	case LINEAR:
	    result = ge_detect_internal_linear_fit(ratio);
	    break;
	case MEAN:
	    result = ge_detect_internal_mean(ratio);
	    break;
	case TMEAN:
	    result = ge_detect_internal_tmean(ratio);
	    break;
	case MEAN_LINEAR:
	    result = ge_detect_internal_tmean_linear(ratio);
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
    if (last.array)
	free(last.array);
    ge_buffer_clean();
}
