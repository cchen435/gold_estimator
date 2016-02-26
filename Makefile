CC = gcc
#CFLAGS = -g -fopenmp -DDEBUG_GE -DDEBUG_INTERNAL -DDEBUG_BUFFER -static -pg -lgsl -lgslcblas
#CFLAGS = -g -fopenmp -DGE_RESTART -static -pg -lgsl -lgslcblas
#CFLAGS = -O3 -DGE_RESTART=1 -fopenmp -static -lgsl -lgslcblas
CFLAGS = -O3 -fopenmp -static -lgsl -lgslcblas

SRCS = ge_buffer.c 	\
       ge_math.c  	\
       ge_internal.c	\
       ge.c	  	\
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
