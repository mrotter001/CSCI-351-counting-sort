/* assert */
#include <assert.h>

/* EXIT_SUCCESS, rand */
#include <stdlib.h>

/* strtol */
#include <stdio.h>

/* OpenMp include */
#include <omp.h>

static int
csort(unsigned const k,
      unsigned const n,
      unsigned const * const in,
      unsigned       * const out)
{
  unsigned * const count = calloc(k + 1, sizeof(*count));
  if (NULL == count) {
    return -1;
  }
  
  #pragma omp parallel for
  for (unsigned i = 0; i < n; i++) {
    #pragma omp atomic
    count[in[i]]++;
  }

  unsigned total = 0;
  for (unsigned i = 0; i <= k; i++) {
    unsigned const counti = count[i];
    count[i] = total;
    total += counti;
  }

  #pragma omp parallel for
  for (unsigned i = 0; i < n; i++) {
    #pragma omp critical
    out[count[in[i]]] = in[i];
    #pragma omp atomic
    count[in[i]]++;
  }

  free(count);

  return 0;
}

int
main(int argc, char *argv[]) {
  /* Get array size from command line */
  unsigned n = strtol(argv[1], NULL, 10);

  /* Get key size from command line */
  unsigned k = strtol(argv[1], NULL, 10);

  /* Allocate memory */
  unsigned * const a = malloc(n * sizeof(*a));
  unsigned * const b = malloc(n * sizeof(*b));

  /* Populate with random values */
  for (unsigned i = 0; i < n; i++) {
    a[i] = rand() % (1u << k);
  }

  /* Sort array */
  int const ret = csort(1u << k, n, a, b);
  assert(0 == ret);

  /* Validate sorted array */
  for (unsigned i = 1; i < n; i++) {
    assert(b[i] >= b[i - 1]);
  }

  /* Free memory */
  free(a);
  free(b);

  return EXIT_SUCCESS;
}
