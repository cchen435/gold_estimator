
#include<stdio.h>
#include<gsl/gsl_fit.h>
#include "ge_list.h"

extern struct _history history;

void ge_lstsq(double **a, double **b, int *elems)
{
	int i, j;
	int size = history.win_size;

	double tmp;

	/* alloc memory for constructing x, y array */	
	double *x = (double *) malloc(sizeof(double) * size);
	double *y = (double *) malloc(sizeof(double) * size);
	
	*elems = history.base->size;
	/* alloc memory for storing a, b vectors */
	double *tmp_a, *tmp_b;
	tmp_a = (double *) malloc(sizeof(double) * (*elems));
	tmp_b = (double *) malloc(sizeof(double) * (*elems));
	(*a) = tmp_a;
	(*b) = tmp_b;

	memset(x, 0, size * sizeof(double));
	memset(y, 0, size * sizeof(double));
	memset((*a), 0, (*elems) * sizeof(double));
	memset((*b), 0, (*elems) * sizeof(double));


	for ( i = 0; i < *elems; i++ ) {
		/* constructing x, y array from history data, in 
		   fact only for y 
		 */
		struct _hist_elem * curr = history.base; 
		for ( j = 0; j < size; j++) {
			y[j] = curr->array[i];
			x[j] = j + 1.0;
			curr = curr->next;
		}

		gsl_fit_linear(x, 1, y, 1, size, &tmp_b[i], &tmp_a[i], \
				&tmp, &tmp, &tmp, &tmp);
	}
}
