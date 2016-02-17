
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"
#include "buffer.h"

struct _buf_list {
    struct list_head list;
    double *data;
};

LIST_HEAD(buf_list);

struct _hist_buffer history;

int ge_buffer_init(int window_size, int dim)
{
    history.steps = window_size;
    history.curr = 0;
    history.dim = dim;
    history.data = (double *)malloc(sizeof(double) * window_size * dim);
    if (history.data == NULL) {
	perror("initializing error when alloc memory");
	exit(EXIT_FAILURE);
    }
}

bool get_buffer_status()
{
    if (history.curr == history.steps)
	return true;
    else
	return false;
}

int ge_buffer_append(double *buf, int size)
{
    struct list_head *pos;
    struct _buf_list *entry;
    int i = 0;

    if (history.dim != size)
	fprintf(stderr, "array size doesn't match, \
                init value: %d, curr value: %d\n", history.dim, size);

    struct _buf_list *tmp =
	(struct _buf_list *)malloc(sizeof(struct _buf_list));
    if (tmp == NULL) {
	perror("failed to allocate the memory at append");
	ge_buffer_clean();
	exit(EXIT_FAILURE);
    }

    tmp->data = (double *)malloc(size * sizeof(double));
    if (tmp->data == NULL) {
	perror("failed to allocate the memory at append");
	ge_buffer_clean();
	exit(EXIT_FAILURE);
    }

    memcpy(tmp->data, buf, size * sizeof(double));

    if (history.curr < history.steps) {
	list_add_tail(&tmp->list, &buf_list);
	history.curr++;
    } else {
	// get and delete oldest timestep
	entry = list_entry(buf_list.next, struct _buf_list, list);
	list_del(&entry->list);
	free(entry->data);
	free(entry);
	// append the list to the tail
	list_add_tail(&tmp->list, &buf_list);
    }

    if (history.curr == history.steps) {
	list_for_each(pos, &buf_list) {
	    entry = list_entry(pos, struct _buf_list, list);
	    memcpy(&history.data[i * history.dim],
		   entry->data, history.dim * sizeof(double));

	    i++;
	}
    }
}

int ge_buffer_clean()
{
    struct list_head *pos;
    struct _buf_list *entry;
    for (pos = buf_list.next; pos != &buf_list;) {
	entry = list_entry(pos, struct _buf_list, list);
	free(entry->data);
	pos = pos->next;
	list_del(pos);
	free(entry);
    }
}
