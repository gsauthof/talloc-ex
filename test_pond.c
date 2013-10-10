/* Example of growing memory pool using [talloc][1].
 *
 * (i.e. a pool of pools)
 *
 * Use case: tons of small allocations, maximal pool size hard to predict,
 *      everything is freed at the end of a computation
 * 
 * compile:
 *   make test_pond CFLAGS="-g -Wall -std=c99" LDLIBS="-ltalloc
 *
 * 2013, Georg Sauthoff <mail@georg.so>
 *
 *
 * [1]: http://talloc.samba.org/talloc/doc/html/index.html
 */


#include <talloc.h>

#include <inttypes.h>
#include <stdio.h>


struct Pond {
  TALLOC_CTX *root;
  TALLOC_CTX *pool;
  size_t pool_pos;
  size_t pool_size;
  size_t pool_itr;
  size_t alignment;
};
typedef struct Pond Pond;

int pond_create(Pond *p, TALLOC_CTX *parent, size_t pool_size)
{
  p->root = talloc_new(parent);
  if (!p->root) return -1;
  // need to decrement default/specified alignment
  // because of bitwise ops
  if (!p->alignment)
    // by default 4 byte alignment
    p->alignment = 3;
  else
    --p->alignment;
  p->pool_size = pool_size;
  p->pool      = talloc_size(p->root, p->pool_size);
  if (!p->pool) return -1;
  p->pool_pos  = 0;
  p->pool_itr  = 1;
  return 0;
}

void *pond_alloc(Pond *p, size_t x)
{
  size_t n = ( ( (x) + p->alignment) & ~ p->alignment);
  // current pool is full?
  if (p->pool_size - p->pool_pos < n) {
    if (n >  p->pool_size) {
      // allocate oversize allocation out of pool
      return talloc_size(p->root, x);
    } else {
      // allocate new pool
      p->pool = talloc_size(p->root, p->pool_size);
      if (!p->pool) return 0;
      p->pool_pos = 0;
      ++p->pool_itr;
    }
  }
  // allocate from current pool
  void *r = (char*) p->pool + p->pool_pos;
  p->pool_pos += n;
  return r;
}

int pond_destroy(Pond *p)
{
  // recursively free all allocations
  return talloc_free(p->root);
}

void pond_stats(const Pond *p)
{
  printf("pool # %3zu, bytes left: %6zu bytes\n", p->pool_itr,
      p->pool_size - p->pool_pos);
}


int main(int argc, char **argv)
{
  Pond pond = {0};
  // using ridiculously small pool size for testing
  pond_create(&pond, 0, 20 * 8);
  pond_stats(&pond);

  uint64_t *a[100] = {0};
  for (unsigned i = 0; i < 100; ++i) {
    a[i] = pond_alloc(&pond, sizeof(uint64_t));
    *a[i] = (i+1)*(i+1);
    pond_stats(&pond);
  }

  // frees all sub-pools, all oversize allocations - everything
  pond_destroy(&pond);
  return 0;
}

