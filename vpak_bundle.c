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
#include "vpak_bundle.h"
#include "libcmini.h"

static struct vpak_mount_entry* root = NULL;

struct vpak_entry*
vpak_find_by_name(struct vpak_dir_entry* parent,
                  const wchar_t* name,
                  int namesize)
{
  unsigned i;
  for (i = 0; i < parent->nchildren; ++i) {
    struct vpak_entry* entry = parent->children[i];
    if (namesize == entry->namesize &&
        CompareStringW(LOCALE_NEUTRAL,
                       NORM_IGNORECASE,
                       name, namesize,
                       entry->name, entry->namesize) == CSTR_EQUAL)
      return entry;
  }
  return NULL;
}

static
struct vpak_mount_entry*
find_or_create_mount_entry(struct vpak_mount_entry* parent,
                           const wchar_t* name,
                           int namesize)
{
  struct vpak_mount_entry* child = parent ? parent->child : root;
  if (namesize == -1)
    namesize = wcslen(name);
  while (child) {
    if (namesize == child->namesize &&
        CompareStringW(LOCALE_NEUTRAL,
                       NORM_IGNORECASE,
                       name, namesize,
                       child->name, child->namesize) == CSTR_EQUAL)
      break;
    child = child->next;
  }
  if (child)
    return child;
  /* need to create a new entry */
  child = malloc(sizeof(*child));
  child->name = wcsndup(name, namesize);
  child->namesize = wcslen(child->name);
  child->child = NULL;
  child->content = NULL;
  /* insert it into the linked list */
  if (parent) {
    child->next = parent->child;
    parent->child = child;
  } else {
    child->next = root;
    root = child;
  }
  return child;
}

/**
 * peek_component
 *
 * peeks a single component from the path
 * for UNC hostnames (e.g. \\host and \\?\UNC\host) returns host
 * for drives (e.g. C:\ and \\?\C:\) returns C:
 * returns pointer in path to the current component
 * updates path to point past the component
 * sets namesize to the size of component
 * returns NULL if path is empty
 */
static
const wchar_t*
peek_component(const wchar_t** path,
               int* namesize)
{
  const wchar_t* p = *path;
  const wchar_t* e;
  if (p[0] == '\\' &&
      p[1] == '\\' &&
      p[2] == '?' &&
      p[3] == '\\')
  {
    /* NT path */
    p += 4;
    if (p[0] == 'U' &&
        p[1] == 'N' &&
        p[2] == 'C' &&
        p[3] == '\\')
    {
      /* NT UNC path */
      p += 4;
    }
  }
  /* skip any slashes or backslashes */
  while (*p == '\\' || *p == '/')
    ++p;
  /* search for the end */
  for (e = p; *e && *e != '\\' && *e != '/'; ++e)
    /* no op */;
  if (e == p)
    return NULL; /* no components left */
  /* store namesize */
  *namesize = e - p;
  /* skip any trailing slashes or backslashes */
  while (*e == '\\' || *e == '/')
    ++e;
  *path = e;
  return p;
}

void
vpak_add_mount(const wchar_t* path,
               struct vpak_dir_entry* dir_entry,
               struct vpak_file_entry* file_entry)
{
  const wchar_t* name;
  int namesize;
  struct vpak_mount_entry* entry = NULL;
  struct vpak_mount_content* content;
  while (name = peek_component(&path, &namesize))
    entry = find_or_create_mount_entry(entry, name, namesize);
  if (!entry)
    return;
  content = malloc(sizeof(*content));
  content->dir_entry = dir_entry;
  content->file_entry = file_entry;
  content->next = entry->content;
  entry->content = content;
}
