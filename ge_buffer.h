#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdbool.h>


extern int ge_buffer_init(int window_size, int elems);
extern int ge_buffer_append(double *buf, int elems);
extern int ge_buffer_clean();
extern bool ge_buffer_status();
extern int ge_buffer_steps(); 
extern int ge_buffer_dim();
extern double * ge_buffer_data(); 
#endif
