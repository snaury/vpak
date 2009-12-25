/*
    vpak - virtualizing packer
    Copyright (C) 2009  Alexey Borzenkov

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <windows.h>

//#define TEST_PATHNAMES
#define TEST_TREES

#ifdef TEST_PATHNAMES

#if 0
/* test if there's network latency / drive spin-up */
#define TEST_PATHNAME "\\\\AZUREBOX\\den750\\testme.exe\\.."
/* it appears there's none, so it is very safe */
#endif

#if 1
/* test if slashes are converted and redundant dots are removed */
#define TEST_PATHNAME "/testme.txt/some/path./here"
/* hurray, they are! */
#endif

int main(int argc, char** argv)
{
  char test_pathname[MAX_PATH] = { 0 };
  GetFullPathName(TEST_PATHNAME, MAX_PATH, test_pathname, NULL);
  printf("Result: '%s'\n", test_pathname);
  return 0;
}

#endif

#ifdef TEST_TREES
#include "vpak_tree.h"
#include <stdio.h>
#include <time.h>

struct vpak_tree* alloc_int_node(int num)
{
  struct vpak_tree* node = vpak_tree_alloc(sizeof(int));
  *VPAK_TREE_CAST(int*, node) = num;
  return node;
}

int compare_int_node(const void* a, const void* b)
{
  int anum = *(const int*)a;
  int bnum = *(const int*)b;
  return anum - bnum;
}

int search_int_node(const void* a, const void* b)
{
  int anum = *(const int*)a;
  int bnum = (int)b;
  return anum - bnum;
}

void dump_tree_node(struct vpak_tree* node, int prefix)
{
  unsigned i;
  for (i = 0; i < prefix*2; ++i) putchar(' ');
  printf("n: %d\n", *VPAK_TREE_CAST(int*,node));
  for (i = 0; i < prefix*2; ++i) putchar(' ');
  printf("h: %d\n", node->height);
  for (i = 0; i < prefix*2; ++i) putchar(' ');
  printf("b: %d\n", node->balance);
  if (node->l) {
    for (i = 0; i < prefix*2; ++i) putchar(' ');
    printf("l:\n");
    dump_tree_node(node->l, prefix + 1);
  }
  if (node->r) {
    for (i = 0; i < prefix*2; ++i) putchar(' ');
    printf("r:\n");
    dump_tree_node(node->r, prefix + 1);
  }
}

int main(int argc, char** argv)
{
  unsigned i, k;
  struct vpak_tree* root = NULL;
  const struct vpak_tree* node;
  srand(time(NULL));
  for (i = 0; i < 15; ++i) {
    int n = rand() % 10;
    printf("insert %d = %p\n", n, vpak_tree_insert(&root, alloc_int_node(n), compare_int_node));
    if (i == 14)
      k = n;
  }
  dump_tree_node(root, 0);
  node = vpak_tree_find(root, (const void*)k, search_int_node);
  if (!node) {
    printf("node %d not found!\n", k);
    return 1;
  }
  if (*VPAK_TREE_CAST(const int*,node) != k) {
    printf("node mismatch %d != %d\n", *VPAK_TREE_CAST(const int*,node), k);
    return 1;
  }
  return 0;
}

#endif
