/* OpenMP test for Oyranos */
#include <stdio.h>

void mp_func(void)
{
  const n = 100000;
  int i,
      array[n];
#pragma omp parallel for
  for( i = 0; i < n; ++i)
    array[i] = n-i;
  fprintf(stdout, "omp_get_num_procs(): %d\n", omp_get_num_procs());
}
