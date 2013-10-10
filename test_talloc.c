/* Example use of the basic [talloc][1] API.
 *
 *
 * compile:
 *   make test_talloc CFLAGS="-g -Wall -std=c99" LDLIBS="-ltalloc
 *
 * 2013, Georg Sauthoff <mail@georg.so>
 *
 * [1]: http://talloc.samba.org/talloc/doc/html/index.html
 */

#include <talloc.h>

#include <inttypes.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  void *root = talloc_size(0, 1024);
  printf("root = %p = talloc_size(0, 1024)\n", root);
  printf("Total size: %zu\n", talloc_total_size(root));

  uint64_t *child_a = talloc(root, uint64_t);
  printf("child_a = %p = talloc(root, uint64_t)\n", child_a);

  printf("Total size of root: %zu\n", talloc_total_size(root));

  uint64_t *child_b = talloc(root, uint64_t);
  printf("child_b = %p = talloc(root, uint64_t)\n", child_b);

  for (unsigned i = 0; i < 10; ++i) {
    uint64_t *grand_child = talloc_size(child_b, 512);
    printf("grand_child = %p = talloc_size(child_b, 512)\n", grand_child);
  }
  printf("Total size of root: %zu\n", talloc_total_size(root));

  talloc_free(child_b);
  printf("recursive free of child_b\n");
  printf("Total size of root: %zu\n", talloc_total_size(root));

  talloc_free(root);
  printf("recursive free of root\n");

  return 0;
}

/* Example run:

root = 0x4c390a0 = talloc_size(0, 1024)
Total size: 1024
child_a = 0x4c39540 = talloc(root, uint64_t)
Total size of root: 1032
child_b = 0x4c395f0 = talloc(root, uint64_t)
grand_child = 0x4c396a0 = talloc_size(child_b, 512)
grand_child = 0x4c39940 = talloc_size(child_b, 512)
grand_child = 0x4c39be0 = talloc_size(child_b, 512)
grand_child = 0x4c39e80 = talloc_size(child_b, 512)
grand_child = 0x4c3a120 = talloc_size(child_b, 512)
grand_child = 0x4c3a3c0 = talloc_size(child_b, 512)
grand_child = 0x4c3a660 = talloc_size(child_b, 512)
grand_child = 0x4c3a900 = talloc_size(child_b, 512)
grand_child = 0x4c3aba0 = talloc_size(child_b, 512)
grand_child = 0x4c3ae40 = talloc_size(child_b, 512)
Total size of root: 6160
recursive free of child_b
Total size of root: 1032
recursive free of root
==1066== 
==1066== HEAP SUMMARY:
==1066==     in use at exit: 0 bytes in 0 blocks

 */

