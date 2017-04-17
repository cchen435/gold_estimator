#ifndef __GE_TYPE_H_
#define __GE_TYPE_H_


typedef enum {P, L, G} level_t;

//typedef enum {NONE=0, THRESHOLD, TTHRESHOLD, MEAN, TMEAN, LINEAR, MEAN_LINEAR_GLOBAL, MEAN_LINEAR_LOCAL} dethods; 

/* define detecting kernel methods
 * based on threshold, statisitc and linear regression
 * P, L, G respresent granularity
 */
#define NONE         0
#define THRESHOLD_P  1
#define THRESHOLD_L  2
#define STATISTIC_P  3
#define STATISTIC_L  4
#define LINEAR_P     5
#define LINEAR_L     6

#if USE_MPI
#define THRESHOLD_G  7
#define STATISTIC_G  8
#define LINEAR_G     9
#endif 
typedef int dmethods;



#define GE_FAULT	1
#define GE_NORMAL   2

struct _hist_buffer {
    int steps;			// number of target steps
    int curr;			// number of cached steps
    int dim;			// size of each step, just consider 1D array
    double *data;		// acctual data buffer
};

typedef struct {
    int size;
    double *array;
} vec_double_t;

typedef struct {
    int size;
    float *array;
} vec_float_t;

#define log_err(str) fprintf(stderr, "%s", str)

#endif
