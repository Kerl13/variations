#ifndef _XOSHIRO256SS_H
#define _XOSHIRO256SS_H

#include <stdint.h>
#include <gmp.h> /* mpz_t */

void x256ss_self_seed(void);

uint64_t x256ss_next(void);

int x256ss_bit(void);
int x256ss_int(int bound);
int64_t x256ss_i64(int64_t bound);
void x256ss_mpz(mpz_t dest, const mpz_t bound);

#endif
