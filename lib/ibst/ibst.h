#ifndef _IBST_H
#define _IBST_H

#include <stdio.h> /* FILE */

enum ibst_kind {IBST_INTERVAL, IBST_NODE};

typedef struct ibst_s ibst_t;

/* An interval-Binary-Search-Tree is either an interval of integers of the
 * form [inf; sup] or a node with two ibst_t children such that for all x in
 * left child and for all y in right child, we have x + 1 < y. */

struct ibst_s {
  enum ibst_kind kind;
  union {
    struct { int inf; int sup; } interval;
    struct { int size; ibst_t *left; ibst_t *right; } node;
  } desc;
};

/* --- Constructors and destructor --- */

ibst_t *ibst_node(ibst_t* left, ibst_t* right);
ibst_t *ibst_interval(int inf, int sup);
void ibst_free(ibst_t*);

/* --- Accessing elements --- */

int ibst_size(const ibst_t*);
int ibst_empty(const ibst_t*);
int ibst_pop_nth(ibst_t**, int n);

/* --- Pretty printing (debug) --- */

void ibst_print(FILE*, const ibst_t*);

#endif
