#ifndef __GE_DETECT_INTERNAL_H_
#define __GE_DETECT_INTERNAL_H_

/**
 * API declaration of ge_detect_internal_* routines
 */

#include "common.h"

int ge_detect_internal_linear(struct _hist_buffer history, double *data, double threshold);
//int ge_detect_internal_tmean(vec_double_t ratio);
//int ge_detect_internal_tmean_linear(vec_double_t ratio, int step);
#endif
