#ifndef _VARIATION_H
#define _VARIATION_H

#include <stdio.h> /* FILE */
#include <gmp.h> /* mpz_t */

/* Counting */
void count_variations(mpz_t *dest, int n);

/* Pretty-printing */
void print_variation(FILE*, const int *var, int n);

/* Random sampling */
void rand_variation_rec(const mpz_t *v, int *dest, int n);
void rand_variation(int *dest, int n);

/* Unranking */
void unrank_variation(const mpz_t *v, int *dest, int n, mpz_t rank);

#endif
