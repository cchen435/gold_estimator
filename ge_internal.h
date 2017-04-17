#ifndef __GE_DETECT_INTERNAL_H_
#define __GE_DETECT_INTERNAL_H_

/**
 * API declaration of ge_detect_internal_* routines
 */

#include "common.h"

int ge_detect_internal_threshold_p(vec_double_t ratio);
int ge_detect_internal_threshold_l(vec_double_t ratio);
int ge_detect_internal_threshold_g(vec_double_t ratio);
int ge_detect_internal_statistic_p(vec_double_t ratio);
int ge_detect_internal_statistic_l(vec_double_t ratio);
int ge_detect_internal_statistic_g(vec_double_t ratio);
int ge_detect_internal_linear_p(vec_double_t ratio);
int ge_detect_internal_linear_l(vec_double_t ratio);
int ge_detect_internal_linear_g(vec_double_t ratio);
//int ge_detect_internal_tmean(vec_double_t ratio);
//int ge_detect_internal_tmean_linear(vec_double_t ratio, int step);
#endif
