
CC = gcc
CFLAGS = -lgsl -lgslcblas

SRCS = ge_list.c \
	ge_math.c \
	test.c
OBJS = $(SRCS:.c=.o)

%.o: %.c
	$(CC) -c $< -o $@


test: $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)
