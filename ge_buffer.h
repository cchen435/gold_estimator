#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdbool.h>

#include "common.h"

extern void GE_Buffer_Init(struct _hist_buffer *history, int window_size, int elems);
extern void ge_buffer_append(GE_dataset *, double *buf, int elems);
extern void ge_buffer_clean(GE_dataset *);
extern bool ge_buffer_status(struct _hist_buffer);
extern int ge_buffer_steps(struct _hist_buffer); 
extern int ge_buffer_dim(struct _hist_buffer);
extern double * ge_buffer_data(); 
#endif
