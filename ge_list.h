#ifndef __GE_LIST_H_
#define __GE_LIST_H_

#include <stdbool.h>

/* store the data for a time step */
struct _hist_elem {
    int size;			// size of the array
    double *array;		// pointer to an array storing data for a time step
    struct _hist_elem *next;	// pointer to buf for next time step
};

/* recording the history data for a given window */
struct _history {
    int win_size;		// window size
    int cur_size;		// current recorded time steps
    struct _hist_elem *base;	// pointer to oldes time step
    struct _hist_elem *front;	// pointer to newerest time step
};

extern void ge_list_init(int window_size);
extern void ge_list_append(double *buf, int size);
extern void ge_list_clean();
extern bool ge_list_get_status();

#endif
