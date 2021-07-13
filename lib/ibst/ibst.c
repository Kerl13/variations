#include "ibst.h"

#include <malloc.h> /* malloc, free */
#include <stdio.h> /* FILE, stdout, fprintf */


/* --- Constructors and destructor --- */

ibst_t *ibst_node(ibst_t *left, ibst_t *right) {
  ibst_t *new = malloc(sizeof(ibst_t));

  new->kind = IBST_NODE;
  new->desc.node.size = ibst_size(left) + ibst_size(right);
  new->desc.node.left = left;
  new->desc.node.right = right;

  return new;
}

ibst_t *ibst_interval(int inf, int sup) {
  ibst_t *new = malloc(sizeof(ibst_t));

  new->kind = IBST_INTERVAL;
  new->desc.interval.inf = inf;
  new->desc.interval.sup = sup;

  return new;
}

void ibst_free(ibst_t *t) {
  if (t->kind == IBST_NODE) {
    ibst_free(t->desc.node.left);
    ibst_free(t->desc.node.right);
  }
  free(t);
}

/* --- Accessing elements --- */

int ibst_size(const ibst_t *t) {
  switch (t->kind) {
    case IBST_INTERVAL:
      return t->desc.interval.sup - t->desc.interval.inf + 1;
    case IBST_NODE:
      return t->desc.node.size;
  }
  return -1;
}

int ibst_empty(const ibst_t *t) {
  return (t->kind == IBST_INTERVAL)
         && (t->desc.interval.inf > t->desc.interval.sup);
}

int ibst_pop_nth(ibst_t **t, int n) {
  int res;
  switch ((*t)->kind) {
    case IBST_INTERVAL:
      res = (*t)->desc.interval.inf + n;
      if (res == (*t)->desc.interval.inf) (*t)->desc.interval.inf++;
      else if (res == (*t)->desc.interval.sup) (*t)->desc.interval.sup--;
      else {
        int sup = (*t)->desc.interval.sup;
        (*t)->desc.interval.sup = res - 1;
        *t = ibst_node(*t, ibst_interval(res + 1, sup));
      }
      break;
    case IBST_NODE: {
      int lsize = ibst_size((*t)->desc.node.left);
      (*t)->desc.node.size--;
      if (n < lsize) {
        res = ibst_pop_nth(&((*t)->desc.node.left), n);
        if (ibst_size((*t)->desc.node.left) == 0) {
          ibst_t *tmp = *t;
          free((*t)->desc.node.left);
          *t = (*t)->desc.node.right;
          free(tmp);
        }
      } else {
        res = ibst_pop_nth(&((*t)->desc.node.right), n - lsize);
        if (ibst_size((*t)->desc.node.right) == 0) {
          ibst_t *tmp = *t;
          free((*t)->desc.node.right);
          *t = (*t)->desc.node.left;
          free(tmp);
        }
      }
      break;
    }
    default:
      res = -1;
  }
  return res;
}

/* --- Pretty printing (debug) --- */

void ibst_print(FILE *f, const ibst_t *t) {
  if (!f) f = stdout;

  switch (t->kind) {
    case IBST_INTERVAL:
      fprintf(f, "[%d, %d]", t->desc.interval.inf, t->desc.interval.sup);
      break;
    case IBST_NODE:
      fprintf(f, "Node(size=%d, ", t->desc.node.size);
      ibst_print(f, t->desc.node.left);
      fprintf(f, ", ");
      ibst_print(f, t->desc.node.right);
      fprintf(f, ")");
      break;
  }
}
