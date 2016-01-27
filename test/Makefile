
CC = gcc
FORTRAN = gfortran
CFLAGS = -I../ -static -g
LDFLAGS = -L../ -lge -lgsl -lgslcblas -lm


all: test thresh_test linear_test fortran_test

test:
	$(CC) -o $@ test.c $(CFLAGS) $(LDFLAGS)

thresh_test:
	$(CC) -o $@ ge_test.c $(CFLAGS) $(LDFLAGS)

linear_test:
	$(CC) -o $@ ge_test2.c $(CFLAGS) $(LDFLAGS)

fortran_test:
	$(FORTRAN) -I../ -o $@ ge_test_f.f90 $(LDFLAGS)

clean:
	rm test thresh_test linear_test
