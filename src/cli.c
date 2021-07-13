#include <time.h>
#include <malloc.h>
#include <sys/random.h>
#include <assert.h>
#include "../lib/xoshiro/xoshiro256ss.h"
#include "variation.h"

static void count_and_print(mpz_t *dest, int n) {
  clock_t start, end;

  printf("Computing the number of variations of size k ≤ %d…\n", n - 1);
  fflush(stdout);
  start = clock();
  count_variations(dest, n);
  end = clock();

  printf("Done in %lfs\n", ((double)(end - start)) / CLOCKS_PER_SEC);
  printf("First values of the sequence:\n");
  for (int i = 0; i < n && i < 20; i++) {
    mpz_out_str(stdout, 10, dest[i]);
    printf("\n");
  }
}

static void sample_rej(int n) {
  clock_t start, end;
  int *dest = calloc(n, sizeof(int));

  printf("Sampling a uniform variation of size %d\n", n);
  fflush(stdout);
  start = clock();
  rand_variation(dest, n);
  end = clock();

  printf("Done in %lfs\n", ((double)(end - start)) / CLOCKS_PER_SEC);
  if (n <= 1000) print_variation(stdout, dest, n);

  free(dest);
}

static void sample_rec(const mpz_t *var, int n) {
  clock_t start, end;
  int *dest = calloc(n, sizeof(int));

  printf("Sampling a uniform variation of size %d\n", n);
  fflush(stdout);
  start = clock();
  rand_variation_rec(var, dest, n);
  end = clock();

  printf("Done in %lfs\n", ((double)(end - start)) / CLOCKS_PER_SEC);
  if (n <= 1000) print_variation(stdout, dest, n);

  free(dest);
}

static void unrank_all(const mpz_t *var, int n) {
  mpz_t rank;
  mpz_init(rank);
  int *dest = calloc(n, sizeof(int));

  printf("All variations of size %d:\n", n);

  while (mpz_cmp(rank, var[n]) < 0) {
    unrank_variation(var, dest, n, rank);
    print_variation(stdout, dest, n);
    mpz_add_ui(rank, rank, 1);
  }

  mpz_clear(rank);
  free(dest);
}

static void unrank_random(const mpz_t *var, int n) {
  clock_t start, end;
  mpz_t rank;
  mpz_init(rank);
  int *dest = calloc(n, sizeof(int));

  printf("Generating a uniform variation of size %d:\n", n);

  x256ss_mpz(rank, var[n]);
  start = clock();
  unrank_variation(var, dest, n, rank);
  end = clock();
  printf("Done in %lfs\n", ((double)(end - start)) / CLOCKS_PER_SEC);

  if (n < 1000) print_variation(stdout, dest, n);

  mpz_clear(rank);
  free(dest);
}

int main() {
  x256ss_self_seed();

  /* Counting */
  int n = 50000;
  mpz_t *var = calloc(n + 1, sizeof(mpz_t));
  for (int i = 0; i <= n; i++) mpz_init(var[i]);
  count_and_print(var, n + 1);

  /* Random sampling */
  printf("--- [ Recursive method ]---\n");
  sample_rec((const mpz_t*)var, n);
  printf("--- [ Rejection method ]---\n");
  sample_rej(1e6);

  /* Unranking */
  printf("--- [ Unranking ] ---\n");
  unrank_all((const mpz_t*)var, 3);
  unrank_random((const mpz_t*)var, 5000);

  free(var);

  return 0;
}
