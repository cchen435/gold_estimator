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

/** GE Internal API definition*/
/**
 * GE_Internal_Linear - detect faults using linear fit method
 * @ratio, change ratio of current timestep to prev timestep
 *
 * The method treat the change ratio of successive timesteps as a linear
 * function, using the history data to estimate the linear parameters for
 * each location
 */
int GE_Internal_Linear(struct _hist_buffer history, double *data,
                              double threshold) {
  double *x, *buffer;
  double a, b, predict, stdv, range, err, theta, sumsq;
  double max, max_err, max_range, max_predict, max_a, max_b, max_theta, max_sumsq, max_stdv;
  int i, steps, elems, res = GE_NORMAL;
  int loc = -1;

  steps = history.steps;
  elems = history.dim;
  buffer = history.data;

  x = (double *)malloc(steps * sizeof(double));
  if (x == NULL) {
    log_err("linear_fit, mem allocation error");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < steps; i++)
    x[i] = i*1.0;

// check whether a fault detected
  max = 0.0;
  for (i = 0; i < elems; i++) {
      // skip if data is 0;
      if (data[i] == 0.0) continue;

    stdv = ge_stdv(&buffer[i], elems, steps);
    sumsq = ge_lstsq(x, &buffer[i], &a, &b, elems, steps);
    // skip if predict is 0.0
    if (a == 0.0 && b == 0.0) continue;
    //predict = a * (steps + 1) + b;
    predict = a * steps + b;

    if (elems > 20)
        range = ge_range(data, 1, elems);
    else 
        range = ge_range(&buffer[i], elems, steps);

    if (range == 0.0) continue;

    err = fabs(data[i] - predict);
    theta = fabs(threshold * range);
    double tmp = err/range;
    if (tmp > max) {
        max = tmp;
        loc = i;
        max_err = err; 
        max_range = range;
        max_theta = theta;
        max_predict = predict;
        max_a = a;
        max_b = b;
        max_stdv = stdv;
        max_sumsq = sumsq;
    }

    // if (fabs(data[i] - predict) > threshold * stdv) {
    if (fabs(predict) > 1e-10 && err > fabs(threshold * range)) {
      res = GE_FAULT;
#if 0
      break;
#endif
    }
  }
  if (loc >= 0) {
      fprintf(stdout, "\n\n\n   [predict statistic (%d)]: \n        history: ", loc);
      for (i = 0; i < steps; i++)
        fprintf(stdout, "%d: %.12f ", i, buffer[loc + i * elems]);
      fprintf(stdout, "\n       [a]: %.12f, [b]: %.12f \n       [predict]: %.12f, [observed]: %.12f \n       "
              "[err]: %.12f, [range]: %.12f\n       [theta]: %.12f, [err/range]: %.12f , [err/stdv]: %.12f , [err/sumsq]: %.12f", 
              max_a, max_b, max_predict, data[loc], max_err, max_range, max_theta, max_err/max_range, max_err/max_stdv, max_err/max_sumsq);
      fprintf(stdout, "\n\n\n");
  } else if (elems == 1) {
      fprintf(stdout, "\n\n\n   [predict statistic (%d)]: \n        history: ", 0);
      for (i = 0; i < steps; i++)
        fprintf(stdout, "%d: %.12f ", i, buffer[i * elems]);
      fprintf(stdout, "\n       [a]: %.12f, [b]: %.12f \n       [predict]: %.12f, [observed]: %.12f \n       "
              "[err]: %.12f, [range]: %.12f\n       [theta]: %.12f , [err/range]: %.12f , [err/stdv]: %.12f , [err/sumsq]: %.12f", 
              a, b, predict, data[0], err, range, theta, err/range, err/stdv, err/sumsq);
      fprintf(stdout, "\n\n\n");
      fflush(stdout);
  }

  return res;
}
