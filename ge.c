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
#if USING_MPI
extern double timer();
#else
#if 0
double timer()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double)tp.tv_sec + ((double)tp.tv_usec) * 1e-6);
}
#endif
#endif
#endif


/* GE API definition */
/**
 * GE_Init -- initialize the library
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

/**
 * GE_Protect -- specify the variable to be protected, and related parameters
 * for it
 * @varname, variable name
 * @var, memory address of variable
 * @datatype, specify the data type of data element
 * @size: variable array size; considered it as 1D array
 * @thresh: threshold value used for threshold method
 *          or threshold to measure the difference between
 *          and actual value
 * @window: window size of buffer
 * @method: the detection method, currently support LINEAR_L and LINEAR_P
 * @use_chg_ratio: working on change ratio or raw data
 * @granularity: used only for LINEAR_P method to indicate how to group data
 * elements
 */
void GE_Protect(char *varname, void *var, int data_type, int size,
                double threshold, int window, int method, int use_chg_ratio,
                int granularity) {
  GE_dataset *dataset = (GE_dataset *)malloc(sizeof(GE_dataset));

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

/*
 * GE_Protect_F -- GE_Protect API for Fortran
 * As for fortran, the address of the variable 
 * passed to the function will be changed, 
 * so need to pass it to Snapshot API in each iteration 
 */

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

/* 
 * GE_Snapshot -- API called at each iteratio to predict 
 * the value of currentstep, and compare it to observed 
 * value for fault detection. It firstly calc the change 
 * ratio (if use_chg_ratio is true), and then append the
 * data to history buf. otherwise quit the application
 * It iterates all resgistered variables
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

/* 
 * GE_Snapshot_1var_F -- Fortran API for GE_Snapshot, it works on one variable 
 */
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

/*
 * GE_Verify_d -- predict and compare the prediction with observed data to 
 * flag faults. GE_Snapshot and GE_Snapshot_1var_F rely on it
 */ 
int GE_Verify_d(GE_dataset *dataset, void *var) {
  assert(dataset != NULL);
  int method = dataset->method;
  int buf_size = dataset->buf_size;
  int array_size = dataset->array_size;
  int granularity = dataset->granularity;
  int use_chg_ratio = dataset->use_chg_ratio;

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
    if (dataset->last == NULL) {
      dataset->last = tmp;
      return GE_NORMAL;
    }
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
    for (i = 0; i < buf_size; i++)
      workset[i] = (tmp[i] - last[i]) / (fabs(last[i]) + 1);

    // update the last
    free(last);
    dataset->last = tmp;
  } else {  // end of use_chg_ratio and start of working on raw data
    workset = tmp;
  }

  mean = ge_mean(workset, 1, buf_size);
  // check whether the buffer is full
  int full = GE_Buffer_Status(dataset->history);
  if (!full) {
    result = GE_NORMAL;
  } else {
    switch (method) {
      case LINEAR_L:
        result = GE_Internal_Linear(dataset->history, &mean,
                                           dataset->threshold);
        break;
      case LINEAR_P:
        result = GE_Internal_Linear(dataset->history, workset,
                                           dataset->threshold);
        break;
      default:
        perror("Unknown method\n");
        exit(EXIT_FAILURE);
    }
  }

    switch (method) {
      case LINEAR_L:
        GE_Buffer_Append(dataset, &mean, 1);
        break;
      case LINEAR_P:
        GE_Buffer_Append(dataset, workset, buf_size);
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
  short *res_buffer = (short *) malloc(sizeof(short) * i);
  if (res_buffer == NULL) {
    perror("allocate memory for res buffer failed\n");
    exit(EXIT_FAILURE);
  }
  memset(res_buffer, 0, sizeof(short) * i);
  MPI_Reduce(manager.result, res_buffer, i, MPI_SHORT, MPI_LOR, 0, MPI_COMM_WORLD);

  if (manager.rank != 0)
      return;

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

  char fname[32] = "GE_Result.txt";
  FILE *fp = fopen(fname, "a+");
  if (fp == NULL) {
    perror("open GE_Result.txt error\n");
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
  
  // free buffer allocated for each dataset 
  while (pp != NULL) {
    if (pp->last) free(pp->last);
    GE_Buffer_Clean(pp);
    free((pp->history).data);
    GE_dataset *tmp = pp;
    pp = pp->next;
    free(tmp);
  }

  free(manager.result);
}
