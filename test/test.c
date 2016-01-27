#include <stdio.h>

#include "ge_list.h"
#include "ge_math.h"

int main (int argc, char *argv) 
{
	int i = 0, j = 0, elems = 0;
	double buf[10];
	double *a, *b;

	ge_list_init(3);

	for (i = 0; i < 3; i++) {
		printf("\nbuf[%d]: ", i);
		for ( j = 0; j < 10; j++) {
			buf[j] = 3 * i + (j+1);
			printf("%f ", buf[j]);
		}
		ge_list_append(buf, 10);
	}

	a = b = NULL;
	ge_lstsq(&a, &b, &elems);

	for (i = 0; i < elems; i++)
		printf("\nid=%d: a=%f, b=%f", i, a[i], b[i]);
	printf("\n");
	ge_list_clean();
}
