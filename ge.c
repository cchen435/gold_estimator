#if OMP
#include <omp.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>


#include "ge_math.h"
#include "ge_internal.h"
#include "common.h"
#include "ge_buffer.h"

#if USE_MPI
#include <mpi.h>
#endif

extern dmethods method;
extern double threshold;
extern struct _hist_buffer history;
extern int ge_freq;

#if USE_MPI
extern int ge_rank;
extern int ge_comm_size;
#endif 

#if DEBUG
//#if USING_MPI
extern double timer();
//#else
#if 0
double timer()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double)tp.tv_sec + ((double)tp.tv_usec) * 1e-6);
}
#endif
#endif


struct {
	int num;
	//int tsteps;
	int fsteps[1024];
} faults;

/**
 * recording the data from last timestep
 * used for calcing change ratio
 */
vec_double_t last;

/* GE API definition */
/**
 * ge_detect_init -- initialize the library
 * @dmethod: the detection method 
 * @win_size: window size of buffer
 * @thresh: threshold value used for threshold method
 *          or threshold to measure the difference between 
 *          and actual value 
 */
void ge_detect_init(dmethods m, int array_size, int win_size, double thresh,
		    int freq)
{
	faults.num = 0;
	//faults.tsteps = 0; 
	memset(faults.fsteps, 0, 512 * sizeof(int));

	method = m;
	threshold = thresh;
	ge_freq = freq;

	switch (m) {
	case LINEAR_P:
	case STATISTIC_P:
		if (threshold < 1.0) {
			log_err ("for linear, mean, tmean, mean_linear, threhsold"
			     "represnts number of stdvs, be larger then 1.\n");
			exit(EXIT_FAILURE);
		}
		ge_buffer_init(win_size, array_size);
		break;
	case LINEAR_L:
	case STATISTIC_L:
		if (threshold < 1.0) {
			log_err ("detect_init: for linear, mean, tmean, mean_linear,"
			     "threhsold represnts number of stdvs,"
			     "be larger then 1");
			exit(EXIT_FAILURE);
		}
		ge_buffer_init(win_size, 1);
		break;
	default:
		log_err("detect_init: undefined method\n");
	}
}

/* verify the current state. first calc the 
   change ratio, then evaluate the change ratio
   if no fault detected, append the change ratio 
   to history buf. otherwise quit the application
   it should be an MPI version for sync quit 
 */
int ge_detect_verify(double *buf, int buf_size, int step)
{
	int i, result;
	vec_double_t ratio, mean_ratio;

	/* used for checking false positives */
	int tmp_step, size;
	double tmp_mean, mean;

#if DEBUG
	double abst, malloct, startt, endt, middlet, ratiot;
	startt = timer();
#endif	
	//faults.tsteps++;

	double *tmp = (double *)malloc(sizeof(double) * buf_size);
	if (tmp == NULL) {
		log_err("detect_verify at line 96: alloc memory error\n");
		exit(-1);
	}

#if DEBUG
	malloct = timer();
#endif
	
	// the first step, no record for calcing the change ratio
	if (last.array == NULL) {
		// process based on absolute value to avoid divide by zeros
		for (i = 0; i < buf_size; i++) {
			tmp[i] = buf[i];
		}
		last.array = tmp;
		return GE_NORMAL;
	}
#if DEBUG
	abst = timer();
#endif
	ratio.array = (double *)malloc(sizeof(double) * buf_size);
	if (ratio.array == NULL) {
		log_err("detect_verify at line 158: alloc memory error\n");
		exit(-1);
	}
	ratio.size = buf_size;
	
    mean_ratio.array = (double *)malloc(sizeof(double));
	if (mean_ratio.array == NULL) {
		log_err("detect_verify at line 165: alloc memory error\n");
		exit(-1);
	}
	mean_ratio.size = 1;

	// calc change ratio
#if OMP
	#pragma omp parallel for 
#endif
	for (i = 0; i < buf_size; i++) {
		tmp[i] = buf[i];
		ratio.array[i] = (tmp[i] - last.array[i])/ (fabs(last.array[i]) + 1);
	}

    switch(method) {
        case THRESHOLD_L:
        case STATISTIC_L:
        case LINEAR_L:
            mean_ratio.array[0] = ge_mean(ratio.array, 1, ratio.size);
            break;
    }

#if DEBUG
	ratiot = timer();
#endif
	// update the last time step data
	free(last.array);
	last.array = tmp;

#if DEBUG
	middlet = timer();
#endif
	// call related method
	switch (method) {
	case THRESHOLD_P:
		result = ge_detect_internal_threshold(ratio);
		break;
	case THRESHOLD_L:
		result = ge_detect_internal_threshold(mean_ratio);
		break;
	case STATISTIC_P:
		result = ge_detect_internal_statistic(ratio);
		break;
	case STATISTIC_L:
		result = ge_detect_internal_statistic(mean_ratio);
		break;
	case LINEAR_P:
		result = ge_detect_internal_linear(ratio);
		break;
	case LINEAR_L:
		result = ge_detect_internal_linear(mean_ratio);
		break;
	default:
		log_err("method not defined\n");
	}

#if GE_RESTART
	if (result == GE_FAULT) {
        // check whether the statistic is the same with before,
        // if yes, it is an false positive
		mean = ge_mean(buf, 1, buf_size);
			
        /* first assume file exists, and try to open a file
		 * with 'read' mode, if failed means file not exist
		 * and open it with write operation
		 */
        char fname[64] = "ge_detect_mean.txt";
        FILE *fp = fopen(fname, "r+");
        if (fp == NULL)
            fp = fopen(fname, "w+");

		/** check whether a fault has been record 
		 *  if it has been recorded (timestep) and the statitic value 
		 *  is the same with current statitic, we consider current fault 
		 *  as false positive and continue
		 */
		while (!feof(fp)) {
			size = fscanf(fp, "%d %lf\n", &tmp_step, &tmp_mean);
			if (size == 0)
				break;
			if (tmp_step == step && fabs(tmp_mean - mean) < 0.00001) {
				result = GE_NORMAL;
				break;
			}
		}
			// no match record, means the fault first appears, record it
		if (result == GE_FAULT)
			fprintf(fp, "%d %f\n", step, mean);
		fclose(fp);
	}

#endif
	if (result == GE_FAULT) {
        //faults.fsteps[faults.num]=faults.tsteps;
		faults.fsteps[faults.num] = step;
		faults.num++;
	}
	//free up the ratio array
	free(ratio.array);
#if DEBUG
	endt = timer();
	printf("\nmalloc: %f, abs: %f, ratio: %f, middle: %f, end: %f\n\n", \
			(malloct-startt), (abst-malloct), (ratiot-abst), (middlet-startt), (endt-startt));

#endif

	return result;
}

void ge_detect_finalize()
{
	int i = 0;
	if (last.array)
		free(last.array);
	ge_buffer_clean();

#if USE_MPI
    int num;
    int j, k;
    int recvbuf[1024];
    if (ge_rank != 0) {
       MPI_Send(&faults.num, 1, MPI_INT, 0, ge_rank, MPI_COMM_WORLD);
       MPI_Send(faults.fsteps, faults.num, MPI_INT, 0, ge_rank, MPI_COMM_WORLD);
    } else {
        for ( i = 1; i < ge_comm_size; i++) {
            MPI_Recv(&num, 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(recvbuf, num, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for ( j = faults.num, k = 0; k < num; j++, k++ )
                faults.fsteps[j] = recvbuf[k];
                printf("recieving faults %d from PID %d (total %d) %d\n", \
                        recvbuf[k], i, num, faults.fsteps[j]);
            faults.num += num;
        } 
    }

    
    if (ge_rank != 0)
        return; 
#endif

	printf("\n\nGE statistic:\nnumber of faults detected: %d\n",
	       faults.num);
	printf("timesteps where faults detected:\n");
	for (i = 0; i < faults.num; i++)
		printf("%d ", faults.fsteps[i]);
	printf("\n");

    char fname[64] = "ge_detect_result.txt";
	FILE *fp = fopen(fname, "a+");
	if (fp == NULL)
		printf("open ge_detect_result.txt error\n");
	fprintf(fp, "Nu: %d\n", faults.num);
	fprintf(fp, "Steps: ");
	for (i = 0; i < faults.num; i++)
		fprintf(fp, "%d,", faults.fsteps[i]);
	fprintf(fp, "\n\n");
	fclose(fp);
}
