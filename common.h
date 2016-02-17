#ifndef __GE_TYPE_H_
#define __GE_TYPE_H_

#define METHOD_MIN	 0
#define THRESHOLD        1
#define MEAN    	 2
#define TMEAN    	 3
#define LINEAR  	 4
#define MEAN_LINEAR	 5
#define METHOD_MAX	 6

#define FAULT	1
#define NORMAL  2

typedef struct {
    int size;
    double *array;
} vec_double_t;

typedef struct {
    int size;
    float *array;
} vec_float_t;

#endif
