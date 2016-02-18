
CC = gcc
FORTRAN = gfortran
CFLAGS = -I../ -static -g
LDFLAGS = -L../ -lge -lgsl -lgslcblas -lm


EXE = ge_math_test 	 	\
      thresh_test 	 	\
      linear_test 	 	\
      fortran_test 	 	\
      gsl_statistics_test 	\
      tmean_test 	 	\
      tmean_linear_test 	\
      ge_buffer_test

all: $(EXE)

ge_math_test:
	$(CC) -o $@ ge_math_test.c $(CFLAGS) $(LDFLAGS)

thresh_test:
	$(CC) -o $@ thresh_test.c $(CFLAGS) $(LDFLAGS)

linear_test:
	$(CC) -o $@ linear_test.c $(CFLAGS) $(LDFLAGS)

fortran_test:
	$(FORTRAN) -I../ -o $@ fortran_test.f90 $(LDFLAGS)

gsl_statistics_test:
	$(CC) -o $@ gsl_statistics.c  $(CFLAGS) $(LDFLAGS)

tmean_test:
	$(CC) -o $@ tmean_test.c  $(CFLAGS) $(LDFLAGS)
tmean_linear_test:
	$(CC) -o $@ tmean_linear_test.c  $(CFLAGS) $(LDFLAGS)

ge_buffer_test:
	$(CC) -o $@ ge_buffer_test.c  $(CFLAGS) $(LDFLAGS)

clean:
	rm $(EXE)
