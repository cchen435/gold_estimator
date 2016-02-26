#ifndef __GE_TYPE_H_
#define __GE_TYPE_H_



typedef enum {NONE, THRESHOLD, TTHRESHOLD, MEAN, TMEAN, LINEAR, MEAN_LINEAR} dmethods; 
//typedef enum {FAULT, NORMAL} ge_status;


#if 0
#define METHOD_MIN	 0
#define THRESHOLD        1
#define MEAN    	 2
#define TMEAN    	 3
#define LINEAR  	 4
#define MEAN_LINEAR	 5
#define METHOD_MAX	 6
#endif 

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
