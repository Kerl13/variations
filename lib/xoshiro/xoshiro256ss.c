#include "xoshiro256ss.h"

#include <stdint.h>
#include <sys/random.h> /* getrandom */
#include <gmp.h> /* mpz_* */

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

static uint64_t s[4];
static uint64_t buf;
static int bufsize = 0;

uint64_t x256ss_next(void) {
	const uint64_t result = rotl(s[1] * 5, 7) * 9;

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 45);

	return result;
}

int x256ss_bit() {
  if (bufsize == 0) {
    buf = x256ss_next();
    bufsize = 64;
  }
  int res = buf & 1;
  bufsize--;
  buf = buf >> 1;
  return res;
}

int x256ss_int(int bound) {
  int x = x256ss_next() & 0x7FFFFFFF;
  int res = x % bound;
  if (x - res > 0x7FFFFFFF - bound + 1) return x256ss_int(bound);
  return res;
}

int64_t x256ss_i64(int64_t bound) {
  int64_t x = x256ss_next() & 0x7FFFFFFFFFFFFFFFL;
  int64_t res = x % bound;
  if (x - res > 0x7FFFFFFFFFFFFFFFL - bound + 1) return x256ss_i64(bound);
  return res;
}

void x256ss_self_seed() {
  getrandom(s, 4 * sizeof(uint64_t), GRND_RANDOM);
  bufsize = 0;
}

/* Slow */
static inline int count_leading_zeros(mp_limb_t x) {
  const int s = sizeof(mp_limb_t) << 3;
  int a = 0, b = s - 1, c;
  while (a < b - 1) {
    c = (a + b) >> 1;
    if (x >> (s - c)) b = c;
    else a = c;
  }
  return a;
}

void x256ss_mpz(mpz_t dest, const mpz_t bound) {
  const size_t nblimbs = mpz_size(bound);
  mp_limb_t *d = mpz_limbs_write(dest, nblimbs);
  int lz = count_leading_zeros(mpz_getlimbn(bound, nblimbs - 1));

  do {
    for (size_t k = 0; k < nblimbs - 1; k++)
      d[k] = (mp_limb_t)x256ss_next();
    d[nblimbs - 1] = ((mp_limb_t)x256ss_next()) >> lz;
    mpz_limbs_finish(dest, nblimbs);
  } while (mpz_cmp(dest, bound) >= 0);
}
