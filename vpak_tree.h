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
#ifndef __VPAK_TREE_H__
#define __VPAK_TREE_H__

struct vpak_tree
{
  struct vpak_tree* l;
  struct vpak_tree* r;
  unsigned height;
  int balance;
};

/**
 * VPAK_TREE_CAST
 *
 * macro to get pointer to node's payload
 * type should be pointer type
 * node should be pointer to vpak_tree
 */
#define VPAK_TREE_CAST(type, node) \
  ((type)((node) + 1))

/**
 * vpak_tree_compare_t
 *
 * should return:
 *  <0 if a < b
 *   0 if a = b
 *  >0 if a > b
 * a and b are pointers to tree node's payloads
 */
typedef int (*vpak_tree_compare_t)(const void* a,
                                   const void* b);

/**
 * vpak_tree_alloc
 *
 * allocates tree node with size bytes of payload
 */
extern
struct vpak_tree*
vpak_tree_alloc(unsigned size);

/**
 * vpak_tree_free
 *
 * frees previous allocated tree node
 */
extern
void
vpak_tree_free(struct vpak_tree*);

/**
 * vpak_tree_insert
 *
 * inserts child under root
 * warning: uses recursion
 * warning: does nothing if compare returns 0
 * note: will keep the tree balanced
 * returns child if successful, NULL otherwise
 */
extern
struct vpak_tree*
vpak_tree_insert(struct vpak_tree** root,
                 struct vpak_tree* child,
                 vpak_tree_compare_t compare);

/**
 * vpak_tree_find
 *
 * looks for node under root with specified key
 * node's payload is always first argument to compare
 * key is always second argument to compare
 * this allows for node's payload and key having different types
 */
extern
const struct vpak_tree*
vpak_tree_find(const struct vpak_tree* root,
               const void* key,
               vpak_tree_compare_t compare);

#endif
