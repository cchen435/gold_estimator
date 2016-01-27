CC = gcc
CFLAGS = -g -static -pg -lgsl -lgslcblas

SRCS = ge_list.c  \
       ge_math.c  \
       ge.c	  \
       ge_f.c

STATIC = libge.a

OBJS = $(SRCS:.c=.o)

ALL: $(STATIC)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(STATIC): $(OBJS)
	ar rcs $@ $(OBJS)

clean:
	rm *.o $(STATIC)
