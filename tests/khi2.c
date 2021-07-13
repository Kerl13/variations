#include "../variation.h"

static int max_le_x(const int *var, int n, int x) {
  int j = 0;
  for (int i = 1; i < n; i++) {
    if (var[i] > var[j] && var[i] <= x) j = i;
  }
  return j;
}

int nb_var(int n) {
  return (n == 1) ? 1 : n * (nb_var(n - 1) + 1);
}

static int rank_le_x(const int *var, int n, int x) {
  int j = max_le_x(var, n, x);
}

double chi_square(int n) {

}
