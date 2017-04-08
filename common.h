#ifndef __GE_TYPE_H_
#define __GE_TYPE_H_

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef enum {P, L} level_t;

//typedef enum {NONE=0, THRESHOLD, TTHRESHOLD, MEAN, TMEAN, LINEAR, MEAN_LINEAR_GLOBAL, MEAN_LINEAR_LOCAL} dethods; 

/* define detecting kernel methods
 * based on threshold, statisitc and linear regression
 * P, L, G respresent granularity
 */
#define NONE         0
#define LINEAR_P     1
#define LINEAR_L     2

#define GE_NORMAL   0
#define GE_FAULT	1

// data type
#define GE_FLOAT  1
#define GE_DOUBLE 2

struct _hist_buffer {
    int steps;			// number of target steps
    int curr;			// number of cached steps
    int dim;			// size of each step, just consider 1D array
    double *data;		// acctual data buffer
};

#define VARNAMELEN 64
typedef struct GE_dataset {
    char var_name[VARNAMELEN];
    void *var; // the pointer to variable address
    void *last; // a copy of data of last timestep, if granularity is large than 1, it is averaged on ranularity.
    int array_size;
    int buf_size;
    int data_type; // 1--float, 2--double

    // threshold used for current variable
    double threshold;
    int method;
    int window;
    int use_chg_ratio;
    int granularity; // do average on granularity

    // the buf_list storing history data. using list data 
    // structure for easy to remove oldest time step data
    // and keep the time step order
    struct list_head buf_list;

    // it copy data in buf_list to an array for easy access
    struct _hist_buffer history;
    struct GE_dataset *next;
} GE_dataset;

#define RESULTSIZE 1024
typedef struct GE_manager {
    int rank;
    int currStep;
    short *result;
#if DEBUG
    double *err;
    double *stdv;
#endif 
    size_t resSize;
    GE_dataset *head;
} GE_manager;

GE_manager manager;

#define log_err(str) fprintf(stderr, "%s", str)

#endif
