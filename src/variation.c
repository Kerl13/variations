#include "variation.h"

#include <stdio.h> /* FILE, fprintf, stdout */
#include <malloc.h> /* malloc, free */
#include <gmp.h> /* mpz_* */

#include "../lib/xoshiro/xoshiro256ss.h" /* x256ss_* */
#include "../lib/ibst/ibst.h" /* ibst_* */

/* --- Random generation tools --- */

/* static inline int msb(int n) { */
/*   int c = -1; */
/*   while (n) { c++; n = n >> 1; } */
/*   return c; */
/* } */

/* static int bernoulli_inv_n(int n) { */
/*   n--; */
/*   int k = msb(n); */
/*   int i = k; */

/*   while (i >= 0) { */
/*     int b = x256ss_bit(); */
/*     int x = (n >> i) & 1; */
/*     if (b > x) i = k; */
/*     if (b < x) return 0; */
/*     i--; */
/*   } */
/*   return 1; */
/* } */

static int mpz_bernoulli_inv_succ_n(const mpz_t n) {
  if (mpz_cmp_ui(n, 1) == 0) return 1;

  long int k = mpz_sizeinbase(n, 2) - 1;
  long int i = k;

  while (i >= 0) {
    int b = x256ss_bit();
    int x = mpz_tstbit(n, i);
    if (b > x) i = k;
    if (b < x) return 0;
    i--;
  }
  return 1;
}

/* --- Partial arrays --- */

typedef struct _parray_s parray_t;
typedef struct _itree_node_s node_t;

struct _itree_node_s {
  int label;
  parray_t *left;
  parray_t *right;
};

struct _parray_s {
  int size;
  node_t *node;
};

static parray_t *make_empty(int size) {
  parray_t *tree = malloc(sizeof(parray_t));
  tree->size = size;
  tree->node = NULL;
  return tree;
}

static void itree_free(parray_t *t) {
  if (t->node != NULL) { /* node */
    itree_free(t->node->left);
    itree_free(t->node->right);
    free(t->node);
  }
  free(t);
}

static void insert(parray_t *t, int x, int pos) {
  if (t->node == NULL) { /* leaf */
    t->node = malloc(sizeof(node_t));
    t->node->label = x;
    t->node->left = make_empty(pos);
    t->node->right = make_empty(t->size - pos - 1);
  } else { /* node */
    int lsize = t->node->left->size;
    if (lsize > pos) insert(t->node->left, x, pos);
    else insert(t->node->right, x, pos - lsize);
  }
  t->size--;
}

static int *parray_to_array(int *dest, const parray_t *t) {
  if (t->node == NULL) { /* leaf */
    for (int i = 0; i < t->size; i++) dest[i] = 0;
    return dest + t->size;
  } else {
    int *next = parray_to_array(dest, t->node->left);
    next[0] = t->node->label;
    return parray_to_array(next + 1, t->node->right);
  }
}

/* --- Recursive method --- */

static void rand_var_rec_aux(const mpz_t *v, parray_t *tree, int n, int p) {
  int pos = x256ss_int(n);
  insert(tree, p, pos);
  if (!mpz_bernoulli_inv_succ_n(v[n-1]))
    rand_var_rec_aux(v, tree, n - 1, p + 1);
}

void rand_variation_rec(const mpz_t *v, int *dest, int n) {
  parray_t *tree = make_empty(n);
  rand_var_rec_aux(v, tree, n, 1);
  parray_to_array(dest, tree);
  itree_free(tree);
}

/* --- Rejection method --- */

static void bounded_fy(int *dest, int n, int p) {
  int i, j, tmp;
  for (i = 0; i < n - p; i++) dest[i] = i + 1;
  for (i = n - p; i < n; i++) dest[i] = 0;
  for (i = 0; i < n - 1; i++) {
    j = x256ss_int(n - i) + i;
    tmp = dest[j];
    dest[j] = dest[i];
    dest[i] = tmp;
  }
}

void rand_variation(int *dest, int n) {
  int p;
start:
  p = x256ss_int(n);
  for (int i = p; i > 1; i--) {
    if (x256ss_int(i) > 0) goto start;
    /* if (bernoulli_inv_n(i)) goto start; */
  }
  bounded_fy(dest, n, p);
}

void print_variation(FILE *f, const int *v, int n) {
  if (!f) f = stdout;
  for (int i = 0; i < n - 1; i++) fprintf(f, "%d ", v[i]);
  if (n) fprintf(f, "%d\n", v[n-1]);
  else fprintf(f, "\n");
}

void count_variations(mpz_t *dest, int n) {
  /* dest is assumed to be initialised. */
  mpz_set_ui(dest[0], 0);
  mpz_set_ui(dest[1], 1);
  for (int i = 2; i < n; i++) {
    mpz_add_ui(dest[i], dest[i - 1], 1);
    mpz_mul_ui(dest[i], dest[i], i);
  }
}

/* --- Unranking --- */

void unrank_variation(const mpz_t *v, int *dest, int n, mpz_t rank) {
  ibst_t *S = ibst_interval(1, n);
  int p = 1;
  mpz_t C, r, P;
  mpz_init_set(C, v[n - 1]);
  mpz_init_set(r, rank);
  mpz_init_set_ui(P, 1);

  while (n > 1) {
    /* Invariant (1): C = v_{n-1, p} */
    /* Invariant (2): P = \prod_{k=n-p+1}^{n-1} k */

    /* Is 0 the first element of the array? */
    if (mpz_cmp(r, C) < 0) {
      *(dest++) = 0;

      /* To stay consistent with the else case */
      if (p > 0) mpz_mul_ui(P, P, n - p);

    /* Otherwise: test whether j = 1, 2, … is the first element of the array */
    } else {
      mpz_sub(r, r, C);
      int j = 1;

      if (p > 0) mpz_add(C, C, P);

      while (mpz_cmp(r, C) >= 0) {
        /* Invariant: P = prod(n - max(p,j) + 1, n - 1) */
        mpz_sub(r, r, C);
        if (j >= p) {
          mpz_sub(C, C, P);
          mpz_mul_ui(P, P, n - j);
        }
        j++;
      }

      *(dest++) = ibst_pop_nth(&S, j - 1);
      p = (j > p ? j : p) - 1;
    }

    /* At this point we have P = (n-p) (n-p+1) ⋅⋅⋅ (n-1) */

    n--;
    /* Fix loop invariant (1) */
    mpz_sub(C, C, P);
    mpz_divexact_ui(C, C, n);
    /* Fix loop invariant (2) */
    if (p) mpz_divexact_ui(P, P, n);
  }

  if (p == 1 || mpz_cmp_si(r, 1) == 0) *dest = ibst_pop_nth(&S, 0);
  else *dest = 0;

  ibst_free(S);
  mpz_clears(C, r, P, NULL);
}
