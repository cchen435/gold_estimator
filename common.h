#ifndef __GE_TYPE_H_
#define __GE_TYPE_H_


#define METHOD_MIN	 0
#define THRESHOLD_METHOD 1
#define LINEAR_FIT	 2
#define METHOD_MAX	 3


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
