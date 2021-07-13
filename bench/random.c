#include <malloc.h> /* calloc, free */
#include <stdio.h> /* printf */
#include <time.h> /* clock_t, clock, CLOCK_PER_SEC */

#include "../src/variation.h"

static double bench_n(int n, int repeat) {
  int *v = calloc(n, sizeof(int));
  double total;
  clock_t start, end;

  start = clock();
  for (int i = 0; i < repeat; i++) {
    rand_variation(v, n);
  }
  end = clock();
  total = (double)(end - start) / CLOCKS_PER_SEC;

  free(v);
  return total / repeat;
}

int main() {
  const int n = 1e7, nbpts = 50, step = n / nbpts, repeat = 100;

  for (int k = step; k <= n; k += step) {
    double res = bench_n(k, repeat);
    printf("%d %lf\n", k, res);
  }

  return 0;
}
