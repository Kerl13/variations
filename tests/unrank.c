#include "../src/variation.h"

#include <stdlib.h>
#include <gmp.h>

static int count_zeros(const int *t, int n) {
  int i, c = 0;
  for (i = 0; i < n; i++) if (t[i] == 0) c++;
  return c;
}

static int is_variation(const int *var, int n) {
  int p = count_zeros(var, n);
  if (p == n) return 0;

  int *perm = calloc(n - p, sizeof(int));
  for (int i = 0; i < n - p; i++) perm[i] = 0;

  for (int i = 0; i < n; i++) {
    int j = var[i];
    if (j < 0 || j > n - p || (j && perm[j - 1])) {
      free(perm);
      return 0;
    }
    if (j) perm[j - 1] = 1;
  }

  free(perm);
  return 1;
}

static int lex_lt(const int *u, const int *v, int n) {
  for (int i = 0; i < n; i++) {
    if (u[i] > v[i]) return 0;
    if (u[i] < v[i]) return 1;
  }
  return 0;
}

static int test_unrank_all(const mpz_t *v, int n) {
  int res = 0;
  int *dest1 = calloc(n, sizeof(int));
  int *dest2 = calloc(n, sizeof(int));
  mpz_t rank;
  mpz_init(rank);

  unrank_variation(v, dest1, n, rank);
  mpz_add_ui(rank, rank, 1);
  if (!is_variation(dest1, n)) goto exit;

  while (mpz_cmp(rank, v[n]) < 0) {
    unrank_variation(v, dest2, n, rank);
    if (!is_variation(dest2, n) || !lex_lt(dest1, dest2, n)) goto exit;
    int *tmp = dest1;
    dest1 = dest2;
    dest2 = tmp;
    mpz_add_ui(rank, rank, 1);
  }

  res = 1;
exit:
  free(dest1);
  free(dest2);
  mpz_clear(rank);
  return res;
}

int main() {
  int n = 4, fail = 0;
  mpz_t *v = calloc(n + 1, sizeof(mpz_t));
  for (int i = 0; i <= n; i++) mpz_init(v[i]);
  count_variations(v, n + 1);

  for (int k = 1; k <= n; k++) {
    if (test_unrank_all((const mpz_t*)v, k) == 0) {
      fprintf(stderr, "test_unrank_all failed for k = %d\n", k);
      fail = 1;
      break;
    }
  }

  for (int i = 0; i <= n; i++) mpz_clear(v[i]);
  free(v);
  return fail;
}
