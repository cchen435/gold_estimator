#CC = gcc
CC = mpicc 
GEPATH=/opt/GESDC/
#CFLAGS = -g -fopenmp -DDEBUG_GE -DDEBUG_INTERNAL -DDEBUG_BUFFER -static -pg -lgsl -lgslcblas
#CFLAGS = -g -fopenmp -DGE_RESTART -static -pg -lgsl -lgslcblas
#CFLAGS = -O3 -DUSE_MPI -DGE_RESTART=1 -fopenmp -static -lgsl -lgslcblas
#CFLAGS = -DUSE_MPI -O3 -fopenmp -static -lgsl -lgslcblas
#CFLAGS = -O3 -DOMP=0 -static -lgsl -lgslcblas
CFLAGS = -g -Wall -Wextra -DOMP=0 -static -lgsl -lgslcblas

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

install:
	if [ ! -d "$(GEPATH)" ]; then mkdir $(GEPATH); fi
	if [ ! -d "$(GEPATH)/include" ]; then mkdir $(GEPATH)/include; fi
	if [ ! -d "$(GEPATH)/lib" ]; then mkdir $(GEPATH)/lib; fi
	rm -f $(GEPATH)/include/* $(GEPATH)/lib/*
	cp *.h $(GEPATH)/include/
	cp lib* $(GEPATH)/lib/

uninstall:
	rm -f $(GEPATH)/include/* $(GEPATH)/lib/*
	if [ -d "$(GEPATH)/include" ]; then rmdir $(GEPATH)/include; fi
	if [ -d "$(GEPATH)/lib" ]; then rmdir $(GEPATH)/lib; fi
	if [ -d "$(GEPATH)" ]; then rmdir $(GEPATH); fi
