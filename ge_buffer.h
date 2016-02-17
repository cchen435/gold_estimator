#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdbool.h>

struct _hist_buffer {
    int steps;			// number of target steps
    int curr;			// number of cached steps
    int dim;			// size of each step, just consider 1D array
    double *data;		// acctual data buffer
};

extern int ge_buffer_init(int window_size, int dim);
extern int ge_buffer_append(double *buf, int size);
extern int ge_buffer_clean();
extern bool ge_buffer_status();

#endif
