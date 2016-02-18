#ifndef _LIST_H_
#define _LIST_H_

/**
 * The following codes implement basic list
 * structure, and related operations.
 * They are borrowed from Linux kernel,
 * mainly from include/linux/list.h
 */

#include <stddef.h>

struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void __list_add(struct list_head *new,
			      struct list_head *prev, struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - insert an entry to the list 
 * @new:     new entry
 * @head:    the node where the new will be inereted before
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

/**
 * list_add_tail - similar to list_add, insert an entry after the head
 * @new: new entry to be inserted
 * @head: the nodel where the new will be inserted after
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

/**
 * __list_del - helper function to remove an entry from list
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - remove an entry from list
 * @entry: the entry tobe removed
 */
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#endif
