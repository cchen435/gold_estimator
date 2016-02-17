
CC = gcc
FORTRAN = gfortran
CFLAGS = -I../ -static -g
LDFLAGS = -L../ -lge -lgsl -lgslcblas -lm


all: test thresh_test linear_test fortran_test gsl_statistics_test

test:
	$(CC) -o $@ test.c $(CFLAGS) $(LDFLAGS)

thresh_test:
	$(CC) -o $@ thresh_test.c $(CFLAGS) $(LDFLAGS)

linear_test:
	$(CC) -o $@ linear_test.c $(CFLAGS) $(LDFLAGS)

fortran_test:
	$(FORTRAN) -I../ -o $@ fortran_test.f90 $(LDFLAGS)

gsl_statistics_test:
	$(CC) -o$@ gsl_statistics.c  $(CFLAGS) $(LDFLAGS)

clean:
	rm test thresh_test linear_test fortran_test gsl_statistics_test
