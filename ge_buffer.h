#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdbool.h>

#include "common.h"

extern void GE_Buffer_Init(struct _hist_buffer *history, int window_size, int elems);
extern void GE_Buffer_Append(GE_dataset *, double *buf, int elems);
extern void GE_Buffer_Clean(GE_dataset *);
extern bool GE_Buffer_Status(struct _hist_buffer);
extern int GE_Buffer_Steps(struct _hist_buffer); 
extern int GE_Buffer_Dim(struct _hist_buffer);
extern double * GE_Buffer_Data(); 
#endif
