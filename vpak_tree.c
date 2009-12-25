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
#include "vpak_tree.h"
#include "libcmini.h"

struct vpak_tree*
vpak_tree_alloc(unsigned size)
{
  struct vpak_tree* node = malloc(sizeof(struct vpak_tree) + size);
  node->l = NULL;
  node->r = NULL;
  node->height = 1;
  node->balance = 0;
  return node;
}

void
vpak_tree_free(struct vpak_tree* p)
{
  free(p);
}

static
void
vpak_tree_recalc(struct vpak_tree* node)
{
  unsigned lh = node->l ? node->l->height : 0;
  unsigned rh = node->r ? node->r->height : 0;
  node->height = (lh > rh ? lh : rh) + 1;
  node->balance = rh - lh;
}

static
struct vpak_tree*
vpak_tree_rotate_l(struct vpak_tree* node)
{
  struct vpak_tree* root = node->r;
  node->r = root->l;
  root->l = node;
  vpak_tree_recalc(node);
  vpak_tree_recalc(root);
  return root;
}

static
struct vpak_tree*
vpak_tree_rotate_r(struct vpak_tree* node)
{
  struct vpak_tree* root = node->l;
  node->l = root->r;
  root->r = node;
  vpak_tree_recalc(node);
  vpak_tree_recalc(root);
  return root;
}

static
struct vpak_tree*
vpak_tree_do_insert(struct vpak_tree* node,
                    struct vpak_tree* child,
                    vpak_tree_compare_t compare)
{
  int c;
  if (!node) {
    /* new leaf node */
    child->l = NULL;
    child->r = NULL;
    child->height = 1;
    child->balance = 0;
    return child;
  }
  c = compare(VPAK_TREE_CAST(void*,node), VPAK_TREE_CAST(void*,child));
  if (!c)
    return NULL; /* duplicate entry */
  if (c > 0) {
    struct vpak_tree* l = vpak_tree_do_insert(node->l, child, compare);
    if (!l)
      return NULL; /* bubble up */
    node->l = l;
  } else {
    struct vpak_tree* r = vpak_tree_do_insert(node->r, child, compare);
    if (!r)
      return NULL; /* bubble up */
    node->r = r;
  }
  vpak_tree_recalc(node);
  if (node->balance <= -2) {
    /* left-left or left-right case */
    if (node->l && node->l->balance > 0)
      node->l = vpak_tree_rotate_l(node->l);
    node = vpak_tree_rotate_r(node);
  }
  else if (node->balance >= +2) {
    /* right-right or right-left case */
    if (node->r && node->r->balance < 0)
      node->r = vpak_tree_rotate_r(node->r);
    node = vpak_tree_rotate_l(node);
  }
  return node;
}

struct vpak_tree*
vpak_tree_insert(struct vpak_tree** root,
                 struct vpak_tree* child,
                 vpak_tree_compare_t compare)
{
  struct vpak_tree* node;
  if (!root || !child)
    return NULL; /* sanity check */
  node = vpak_tree_do_insert(*root, child, compare);
  if (!node)
    return NULL; /* bubble up */
  *root = node;
  return child;
}

const struct vpak_tree*
vpak_tree_find(const struct vpak_tree* root,
               const void* key,
               vpak_tree_compare_t compare)
{
  int c;
  while (root) {
    c = compare(VPAK_TREE_CAST(const void*, root), key);
    if (!c)
      break;
    if (c > 0)
      root = root->l;
    else
      root = root->r;
  }
  return root;
}
