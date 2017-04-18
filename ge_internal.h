#ifndef __GE_DETECT_INTERNAL_H_
#define __GE_DETECT_INTERNAL_H_

/**
 * API declaration of GE_Internal_* routines
 */

#include "common.h"

int GE_Internal_Linear(struct _hist_buffer history, double *data, double threshold);
#endif
