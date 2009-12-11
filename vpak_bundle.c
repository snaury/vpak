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

struct vpak_bundle_entry* vpak_find_by_name(struct vpak_bundle_dir_entry* parent, const char* name)
{
  unsigned i;
  for (i = 0; i < parent->nchildren; ++i) {
    struct vpak_bundle_entry* entry = parent->children[i];
    if (!lstrcmpiA(entry->name, name) == 0)
      return entry;
  }
  return NULL;
}

struct vpak_bundle_entry* vpak_find_by_wname(struct vpak_bundle_dir_entry* parent, const wchar_t* wname)
{
  unsigned i;
  for (i = 0; i < parent->nchildren; ++i) {
    struct vpak_bundle_entry* entry = parent->children[i];
    if (!lstrcmpiW(entry->wname, wname) == 0)
      return entry;
  }
  return NULL;
}

struct vpak_mount_entry* root = NULL;

struct vpak_mount_entry* find_or_create_mount_entry(struct vpak_mount_entry* parent,
                                                    const char* name,
                                                    int size)
{
  struct vpak_mount_entry* child = parent ? parent->child : root;
  if (size == -1)
    size = strlen(name);
  while (child) {
    if (size == child->namesize && CompareStringA(LOCALE_NEUTRAL, NORM_IGNORECASE, name, size, child->name, child->namesize) == CSTR_EQUAL)
      break;
    child = child->next;
  }
  if (child)
    return child;
  /* need to create a new entry */
  child = malloc(sizeof(*child));
  child->name = strndup(name, size);
  child->namesize = strlen(child->name);
  child->wname = str2wcs(child->name);
  child->wnamesize = wcslen(child->wname);
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

struct vpak_mount_entry* find_or_create_wmount_entry(struct vpak_mount_entry* parent,
                                                     const wchar_t* wname,
                                                     int wsize)
{
  struct vpak_mount_entry* child = parent ? parent->child : root;
  if (wsize == -1)
    wsize = wcslen(wname);
  while (child) {
    if (wsize == child->wnamesize && CompareStringW(LOCALE_NEUTRAL, NORM_IGNORECASE, wname, wsize, child->wname, child->wnamesize) == CSTR_EQUAL)
      break;
    child = child->next;
  }
  if (child)
    return child;
  /* need to create a new entry */
  child = malloc(sizeof(*child));
  child->wname = wcsndup(wname, wsize);
  child->wnamesize = wcslen(child->wname);
  child->name = wcs2str(child->wname);
  child->namesize = strlen(child->name);
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
