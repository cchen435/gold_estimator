
#include <string.h>
#include "ge.h"

#if 0
void ge_detect_init_(dmethods * method, int *array_size, int *win_size,
		     double *thresh, int *freq)
{
	ge_detect_init(*method, *array_size, *win_size, *thresh, *freq);
}

int ge_detect_verify_(double buf[], int *buf_size, int *step)
{
	int result;
	result = ge_detect_verify(buf, *buf_size, *step);
	return result;
}

void ge_detect_finalize_()
{
	ge_detect_finalize();
}
#endif

void ge_init_c_() { GE_Init(); }

void ge_snapshot_c_() { GE_Snapshot(); }

void ge_printresult_c_() { GE_PrintResult(); }

void ge_finalize_c_() { GE_Finalize(); }

void ge_protect_1d_float_(char *var_name, int *var_name_length, int *r1,
                          float *threshold, int *window, int *method,
                          int *use_chg_ratio, int *granularity) {
  int i;
  char name[*var_name_length + 1];
  for (i = 0; i < *var_name_length; i++) name[i] = var_name[i];
  name[*var_name_length] = '\0';
  GE_Protect_F(name, GE_FLOAT, *r1, *threshold, *window, *method,
               *use_chg_ratio, *granularity);
}

void ge_increase_counter_c_() {
    size_t resSize = manager.resSize;
    manager.currStep += 1;

  if ((manager.currStep+1) % RESULTSIZE == 0) {
    resSize += RESULTSIZE;

    short *tmp = (short *)realloc(manager.result, sizeof(short) * resSize);
    if (tmp == NULL) {
      char msg[128];
      sprintf(msg, "%s (%s-%d): alloc memory error\n", __func__, __FILE__,
              __LINE__);
      log_err(msg);
      exit(EXIT_FAILURE);
    }
    manager.result = tmp;
    manager.resSize = resSize;
#if DEBUG
    double *errtmp = (double *)realloc(manager.err, sizeof(double) * resSize);
    manager.err = errtmp;
    double *stdvtmp = (double *)realloc(manager.stdv, sizeof(double) * resSize);
    manager.stdv = stdvtmp;
#endif
  }
}

void ge_protect_1d_double_(char *var_name, int *var_name_length, int *r1,
                           double *threshold, int *window, int *method,
                           int *use_chg_ratio, int *granularity) {
  int i;
  char name[*var_name_length + 1];
  for (i = 0; i < *var_name_length; i++) name[i] = var_name[i];
  name[*var_name_length] = '\0';
  GE_Protect_F(name, GE_DOUBLE, *r1, *threshold, *window, *method,
               *use_chg_ratio, *granularity);
}

void ge_snapshot_1d_1var_float_(char *var_name, int *var_name_length,
                                float *data) {
  int i;
  GE_dataset *p;
  char name[*var_name_length + 1];
  for (i = 0; i < *var_name_length; i++) name[i] = var_name[i];
  name[*var_name_length] = '\0';

  p = manager.head;
  while (p != NULL) {
    if (strcmp(name, p->var_name) == 0) break;
    p = p->next;
  }

  if (p == NULL) {
    printf("ERROR: cannot find the variable name in data list\n");
    exit(EXIT_FAILURE);
  }
  GE_Snapshot_1var_F(p, data);
}
void ge_snapshot_1d_1var_double_(char *var_name, int *var_name_length,
                                 double *data) {
  int i;
  GE_dataset *p;
  char name[*var_name_length + 1];
  for (i = 0; i < *var_name_length; i++) name[i] = var_name[i];
  name[*var_name_length] = '\0';

  p = manager.head;
  while (p != NULL) {
    if (strcmp(name, p->var_name) == 0) break;
    p = p->next;
  }

  if (p == NULL) {
    printf("ERROR: cannot find the variable name in data list\n");
    exit(EXIT_FAILURE);
  }
  GE_Snapshot_1var_F(p, data);
}
