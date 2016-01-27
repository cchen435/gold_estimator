#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "ge_list.h"

struct _history history;

/* init the history buffer, mainly create the buffer 
   according to the window size 
*/
void ge_list_init(int window_size)
{
	int i;
	if (window_size <= 0) {
		printf("incorrect parameter. window size should be larger than zero");
		exit(-1);
	}

	history.win_size = window_size;
	history.cur_size = 0;
	history.base = (struct _hist_elem *) malloc (sizeof (struct _hist_elem));

	struct _hist_elem * tmp = history.base;
	for ( i = 1; i < window_size; i++) {
		tmp->next = (struct _hist_elem*) malloc(sizeof (struct _hist_elem));
		tmp = tmp->next;
	}
	
	history.front = history.base;
}

/* check wether the history buf if full for estimation */
bool ge_list_get_status() {
	if (history.cur_size == history.win_size)
		return true;
	else 
		return false;
}

/* put the newest data into the history buf, if necessary remove the oldest data */
void ge_list_append(double *buf, int size)
{
	if ( history.cur_size !=0 && history.base->size != size) {
		printf("array size do not match previous one\n");
		exit (-1);
	}

	double *tmp = (double *) malloc(sizeof(double) * size);
	memcpy(tmp, buf, size * sizeof(double));

	if ( history.cur_size < history.win_size ) { // history buf is not full, append to the front
		history.cur_size++;
		history.front->array = tmp;
		history.front->size = size;
		if (history.cur_size < history.win_size)
			history.front = history.front->next;
	} else if (history.cur_size == history.win_size) { // history buf is full, replace the oldest one 
		/* base always points to oldest one, so update it */
		struct _hist_elem * n = history.base; 
		
		assert(n != NULL);
		history.base = n->next;

		/* previous oldest one will be used to store the newest one */
		n->next = NULL;
		free(n->array);
		n->array = tmp;
		n->size = size;

		/* append the newest one to the front */
		history.front->next = n;
		history.front = n;
	}
	
}


void ge_list_clean()
{
	int i;
	struct _hist_elem * tmp;
	for (i = 0; i < history.win_size; i++) {
		tmp = history.base;
		history.base = tmp->next;
		if (tmp != NULL) {
			if (tmp -> array != NULL)
				free(tmp->array);
			free (tmp);
		}
	}
}
