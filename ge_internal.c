#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "ge_math.h"

#if USE_MPI
#include <mpi.h>
#endif

level_t level;

/*
int method = NONE;
double threshold = 0.0;
extern struct _hist_buffer history;
int ge_freq;
*/

#if USE_MPI
int ge_rank;
int ge_comm_size;
#endif

/** GE Internal API definition*/
#if 0
/**
 * ge_dtect_internal_threshold - detect the error using threshold
 * @ratio, change ratio of current timestep to prev timestep
 */
int ge_detect_internal_threshold(vec_double_t ratio) {
  int i;
  for (i = 0; i < ratio.size; i++)
    if (fabs(ratio.array[i]) > threshold) return GE_FAULT;
  return GE_NORMAL;
}

/**
 * ge_detect_internal_mean - using mean method to detect error
 * @ratio, change ratio of current timestep to prev timestep
 *
 * The method compares the current change ratio to the mean
 * value of history change ratio for each loaction
 */
int ge_detect_internal_statistic(vec_double_t ratio) {
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
#endif
/**
 * ge_detect_internal_linear - detect faults using linear fit method
 * @ratio, change ratio of current timestep to prev timestep
 *
 * The method treat the change ratio of successive timesteps as a linear
 * function, using the history data to estimate the linear parameters for
 * each location
 */
int ge_detect_internal_linear(struct _hist_buffer history, double *data,
                              double threshold) {
  double *x, *buffer;
  double a, b, predict, stdv;
  int i, steps, elems, res = GE_NORMAL;

  steps = history.steps;
  elems = history.dim;
  buffer = history.data;

  // fprintf(stderr, "DEBUG: %s (%s:%d): elems: %d\n", __func__, __FILE__,
  // __LINE__, elems);

  x = (double *)malloc(steps * sizeof(double));
  if (x == NULL) {
    log_err("linear_fit, mem allocation error");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < steps; i++)
    x[i] = i;

// check whether a fault detected
#if DEBUG
  double max_err = 0.0;
  double min_err = 100000.0;
  double max_err_ratio = 0.0;
  double min_err_ratio = 100000.0;
  double predict1, predict2;
  double range1, range2;
  int count = 0;
  int loc1, loc2;
  double a1, b1;
  double a2, b2;
#endif
  for (i = 0; i < elems; i++) {
      if (data[i] == 0) continue;

    stdv = ge_stdv(&buffer[i], elems, steps);
    ge_lstsq(x, &buffer[i], &a, &b, elems, steps);
    //predict = a * (steps + 1) + b;
    predict = a * steps + b;
    double range;
    if (elems > 20)
        range = ge_range(data, 1, elems);
    else 
        range = ge_range(&buffer[i], elems, steps);

    // if (fabs(data[i] - predict) > threshold * stdv) {
    if (fabs(predict) > 1e-10 && fabs(data[i] - predict) > fabs(threshold * range)) {
      res = GE_FAULT;
#if DEBUG
      //log_err("fault detected\n");
      double err = fabs(data[i] - predict);
      double err_ratio = err/fabs(predict);
      if (err > max_err) {
          max_err = err;
          loc1 = i;
          predict1 = predict;
          range1 = range;
          a1 = a;
          b1 = b;
      }
      if (err_ratio > max_err_ratio) {
          max_err_ratio = err_ratio;
          loc2 = i;
          predict2 = predict;
          range2 = range;
          a2 = a;
          b2 = b;
      }

      if (min_err > err)
          min_err = err;
      if (min_err_ratio > err_ratio)
          min_err_ratio = err_ratio;

      count++;

      manager.err[manager.currStep] = fabs(data[i] - predict) / data[i];
      manager.stdv[manager.currStep] = stdv;
#else
      break;
#endif
    }
  }
#if DEBUG
  if (res == GE_FAULT) { 
      fprintf(stderr, "\n\n\n        [prediction error statistic: ] max: %f, "
            "max ratio: %f, min: %f, min ratio: %f, points: %f\n", 
            max_err, max_err_ratio, min_err, min_err_ratio, count*1.0/elems);

      fprintf(stderr, "        [max err (%d)]: \n            ", loc1);
      for (i = 0; i < steps; i++)
        fprintf(stderr, "%d: %.12f ", i, buffer[loc1 + i * elems]);
      fprintf(stderr, "\n            [a]: %f, [b]: %f, [predict]: %.12f, [observed]: %.12f, [err]: %.12f, [range]: %.12f ", 
              a1, b1, predict1, data[loc1], fabs(data[loc1]-predict1), range1);

      fprintf(stderr, "\n        [max err ratio (%d)]:\n            ", loc2);
      for (i = 0; i < steps; i++)
        fprintf(stderr, "%d: %.12f ", i, buffer[loc2 + i * elems]);
      fprintf(stderr, "\n            [a]: %f, [b]: %f, [predict]: %.12f, [observed]: %.12f, [err]: %.12f, [range]: %.12f ", 
              a2, b2, predict2, data[loc2], fabs(data[loc2]-predict2), range2);

      fprintf(stderr, "\n\n\n");
  }
#endif 
  return res;
}
