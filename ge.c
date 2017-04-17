#if OMP
#include <omp.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include <mpi.h>

#include "common.h"
#include "ge_buffer.h"
#include "ge_internal.h"
#include "ge_math.h"

extern int ge_freq;

#if DEBUG
//#if USING_MPI
extern double timer();
//#else
#if 0
double timer()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double)tp.tv_sec + ((double)tp.tv_usec) * 1e-6);
}
#endif
#endif

struct {
  int num;
  // int tsteps;
  int fsteps[1024];
} faults;

/* GE API definition */
/**
 * ge_detect_init -- initialize the library
 * @method: the detection method
 * @win_size: window size of buffer
 * @thresh: threshold value used for threshold method
 *          or threshold to measure the difference between
 *          and actual value
 */
int GE_Verify_d(GE_dataset *dataset, void *var);

void GE_Init() {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  manager.rank = rank;

  manager.result = (short *)malloc((RESULTSIZE) * sizeof(short));
  if (manager.result == NULL) {
    char msg[128];
    sprintf(msg, "%s (%s-%d): allocating memory for detection result error\n",
            __func__, __FILE__, __LINE__);
    perror(msg);
    exit(EXIT_FAILURE);
  }
  memset(manager.result, 0, RESULTSIZE*sizeof(short));
  manager.currStep = 1;
  manager.head = NULL;
}

void GE_Protect(char *varname, void *var, int data_type, int size,
                double threshold, int window, int method, int use_chg_ratio,
                int granularity) {
  GE_dataset *dataset = (GE_dataset *)malloc(sizeof(GE_dataset));
  // fprintf(stderr, "DEBUG: %s -- %s:%d\n", __func__, __FILE__, __LINE__);

  if (dataset == NULL) {
    char msg[128];
    sprintf(msg, "%s (%s-%d): allocating memory for detection result error\n",
            __func__, __FILE__, __LINE__);
    perror(msg);
    exit(EXIT_FAILURE);
  }

  sprintf(dataset->var_name, varname);
  dataset->var = var;
  dataset->array_size = size;
  dataset->data_type = data_type;
  dataset->threshold = threshold;
  dataset->window = window;
  dataset->method = method;
  dataset->use_chg_ratio = use_chg_ratio;
  dataset->granularity = granularity;
  dataset->last = NULL;

  dataset->next = manager.head;
  manager.head = dataset;

  dataset->buf_list.prev = dataset->buf_list.next = &(dataset->buf_list);

  // * calculate the bufsize
  int elems = size / granularity;
  if (size % granularity > 0)
      elems += 1;
  dataset->buf_size = elems;
  if (elems == 0 || method == LINEAR_L) {
    elems = 1;
  }

  GE_Buffer_Init(&dataset->history, window, elems);

}

void GE_Protect_F(char *varname, int data_type, int size, double threshold,
                  int window, int method, bool use_chg_ratio, int granularity) {
  GE_dataset *dataset = (GE_dataset *)malloc(sizeof(GE_dataset));
  if (dataset == NULL) {
    char msg[128];
    sprintf(msg, "%s (%s-%d): allocating memory for detection result error\n",
            __func__, __FILE__, __LINE__);
    perror(msg);
    exit(EXIT_FAILURE);
  }

  sprintf(dataset->var_name, varname);
  // as for fortran, the address of the variable passed to the function will be
  // changed,
  // so need to pass it in each iteration
  // dataset->var = var;
  dataset->var = NULL;
  dataset->data_type = data_type;
  dataset->array_size = size;
  dataset->threshold = threshold;
  dataset->window = window;
  dataset->method = method;
  dataset->use_chg_ratio = use_chg_ratio;
  dataset->granularity = granularity;
  dataset->buf_list.prev = dataset->buf_list.next = &(dataset->buf_list);
  dataset->next = NULL;
  dataset->last = NULL;

  // * calculate the bufsize
  int elems = size / granularity;
  dataset->buf_size = elems;
  if (elems == 0 || method == LINEAR_L) {
    elems = 1;
  }

  GE_Buffer_Init(&dataset->history, window, elems);

  dataset->next = manager.head;
  manager.head = dataset;
}

/* verify the current state. first calc the
   change ratio, then evaluate the change ratio
   if no fault detected, append the change ratio
   to history buf. otherwise quit the application
   it should be an MPI version for sync quit
 */

void GE_Snapshot() {
  int res = GE_NORMAL;
  int currStep = manager.currStep;
  size_t resSize = manager.resSize;
  GE_dataset *pp = manager.head;
  int data_type;

  // fprintf(stderr, "DEBUG: %s -- %s:%d\n", __func__, __FILE__, __LINE__);
  while (pp != NULL) {
    data_type = pp->data_type;
    switch (data_type) {
      case GE_DOUBLE:
        res |= GE_Verify_d(pp, NULL);
        break;
      default:
        perror("data_type not supported yet\n");
        exit(EXIT_FAILURE);
    }
    //if (res == GE_FAULT) break;
    pp = pp->next;
  }
  manager.result[currStep] = res;

  currStep++;
  manager.currStep = currStep;

  if ((currStep+1) % RESULTSIZE == 0) {
    resSize += RESULTSIZE;

    short *tmp = (short *)realloc(manager.result, sizeof(short) * resSize);
    if (tmp == NULL) {
      char msg[128];
      sprintf(msg, "%s (%s-%d): alloc memory error\n", __func__, __FILE__,
              __LINE__);
      log_err(msg);
      exit(EXIT_FAILURE);
    }
    memset(tmp+resSize-RESULTSIZE, 0, RESULTSIZE * sizeof(short));
    manager.result = tmp;
    manager.resSize = resSize;
  }
}

void GE_Snapshot_1var_F(GE_dataset *pp, void *var) {
  int currStep = manager.currStep;
  int data_type = pp->data_type;
  int res;
  switch (data_type) {
    case GE_DOUBLE:
      res = GE_Verify_d(pp, var);
      manager.result[currStep] |= res;
      break;
    default:
      perror("data_type not supported yet\n");
      exit(EXIT_FAILURE);
  }
}

// verify on double data type
int GE_Verify_d(GE_dataset *dataset, void *var) {
  assert(dataset != NULL);
    // fprintf(stderr, "DEBUG (%dth step): %s -- %s:%d\n", manager.currStep, __func__, __FILE__, __LINE__);

  // make sure the buf_size is correct
  int method = dataset->method;
  int buf_size = dataset->buf_size;
  int array_size = dataset->array_size;
  int granularity = dataset->granularity;
  int use_chg_ratio = dataset->use_chg_ratio;

  // assert(buf_size == array_size / granularity);

  int i, j, result;
  double *data, *workset, mean;

  if (dataset->var)
    data = (double *)dataset->var;
  else if (var)
    data = (double *)var;
  else {
    perror("NO input data\n");
    exit(EXIT_FAILURE);
  }
  // tmp is to storing a copy of data
  double *tmp = (double *)malloc(sizeof(double) * buf_size);
  if (tmp == NULL) {
    char msg[128];
    sprintf(msg, "%s (%s-%d): alloc memory error\n", __func__, __FILE__,
            __LINE__);
    log_err(msg);
    exit(EXIT_FAILURE);
  }

  // copy data to tmp and do average if necessary
  if (granularity == 1)
    memcpy(tmp, data, buf_size * sizeof(double));
  else {
    for (i = 0, j = 0; i < array_size && j < buf_size; i += granularity, j++) {
        int subsize = granularity;
        if (j == buf_size - 1)
            subsize = array_size - i;
      tmp[j] = ge_mean(&data[i], 1, subsize);
    }
  }

  if (use_chg_ratio) {  // the buf stores change ratios instead of raw data
                        // first time step recording data only
    // fprintf(stderr, "DEBUG (%dth step): %s -- %s:%d\n", manager.currStep, __func__, __FILE__, __LINE__);
    if (dataset->last == NULL) {
      dataset->last = tmp;
      return GE_NORMAL;
    }
    // fprintf(stderr, "DEBUG (%dth step): %s -- %s:%d\n", manager.currStep, __func__, __FILE__, __LINE__);

    // calc and record ratio info
    workset = (double *)malloc(sizeof(double) * buf_size);
    if (workset == NULL) {
      char msg[128];
      sprintf(msg, "%s (%s-%d): alloc memory error\n", __func__, __FILE__,
              __LINE__);
      log_err(msg);
      exit(EXIT_FAILURE);
    }

    double *last = (double *)dataset->last;
    // fprintf(stderr, "DEBUG: %s -- %s:%d\n", __func__, __FILE__, __LINE__);
    for (i = 0; i < buf_size; i++)
      workset[i] = (tmp[i] - last[i]) / (fabs(last[i]) + 1);

    // update the last
    // fprintf(stderr, "DEBUG: %s -- %s:%d\n", __func__, __FILE__, __LINE__);
    free(last);
    dataset->last = tmp;
  } else {  // end of use_chg_ratio and start of working on raw data
    workset = tmp;
  }

  mean = ge_mean(workset, 1, buf_size);
    // fprintf(stderr, "DEBUG: %s -- %s:%d\n", __func__, __FILE__, __LINE__);
  // check whether the buffer is full
  int full = ge_buffer_status(dataset->history);
  if (!full) {
      // fprintf(stderr, "\n\nDEBUG: [currStep: %d]: hist buffer not full\n\n", manager.currStep);
    result = GE_NORMAL;
  } else {
    switch (method) {
      case LINEAR_L:
        result = ge_detect_internal_linear(dataset->history, &mean,
                                           dataset->threshold);
        break;
      case LINEAR_P:
        result = ge_detect_internal_linear(dataset->history, workset,
                                           dataset->threshold);
        break;
      default:
        perror("Unknown method\n");
        exit(EXIT_FAILURE);
    }
  }

    switch (method) {
      case LINEAR_L:
        ge_buffer_append(dataset, &mean, 1);
        break;
      case LINEAR_P:
        ge_buffer_append(dataset, workset, buf_size);
        break;
      default:
        perror("Unknown method\n");
        exit(EXIT_FAILURE);
    }


  // workset is separately allocated when using change ratio
  // otherwise it is a copy of tmp;
  if (use_chg_ratio) free(workset);
  // if not using change ratio, the copy of tmp is not useful, as it has been appended 
  // if use change ratio, need to avoid free it as it saves a copy of data from 
  // last time step
  if (!use_chg_ratio) free(tmp);
  return result;
}

void GE_PrintResult() {
  char *msg;
  int i = manager.currStep;
  int digits = 0;
  while (i > 0) {
    digits++;
    i = i / 10;
  }
#if DEBUG
  int msg_size = manager.currStep * (digits + 14);
#else 
  int msg_size = manager.currStep * (digits + 4);
#endif
  msg = (char *)malloc(msg_size);
  if (msg == NULL) {
    perror("allocate memory for msg failed\n");
    fprintf(stderr, "msg size: %d, currStep: %d\n", msg_size, manager.currStep);
    exit(EXIT_FAILURE);
  }
  sprintf(msg, "%d:%d", 1, manager.result[1]);
  for (i = 2; i < manager.currStep; i++) {
    sprintf(msg, "%s %d:%d", msg, i, manager.result[i]);
  }

  fprintf(stdout, "[GE rank: %d]: %s \n", manager.rank, msg);
  fflush(stdout);

  char fname[64] = "ge_detect_result.txt";
  FILE *fp = fopen(fname, "a+");
  if (fp == NULL) {
    perror("open ge_detect_result.txt error\n");
    exit(EXIT_FAILURE);
  }
  fprintf(fp, "[GE rank: %d]: %s \n", manager.rank, msg);
  fflush(fp);
  fclose(fp);

    free(msg);
    msg = NULL;
}

void GE_Finalize() {
  GE_dataset *pp = manager.head;
  while (pp != NULL) {
    if (pp->last) free(pp->last);
    ge_buffer_clean(pp);
    free((pp->history).data);
    GE_dataset *tmp = pp;
    pp = pp->next;
    free(tmp);
  }

  free(manager.result);
}
