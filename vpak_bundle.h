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
#ifndef __VPAK_BUNDLE_H__
#define __VPAK_BUNDLE_H__

#define VPAK_BUNDLE_TYPE_DIR 0
#define VPAK_BUNDLE_TYPE_FILE 1

/* vpak bundles */

struct vpak_entry
{
  int type;
  wchar_t* name;
  int namesize;
  /* TODO: attributes, creation/modification time */
};

struct vpak_dir_entry
{
  unsigned int nchildren;
  struct vpak_entry* children[];
};

struct vpak_file_entry
{
  unsigned int size;
  unsigned char* data;
};

#define VPAK_BUNDLE_ENTRY_AS_DIR_ENTRY(entry) \
  ((vpak_dir_entry*)((entry) + 1))
#define VPAK_BUNDLE_ENTRY_AS_FILE_ENTRY(entry) \
  ((vpak_file_entry*)((entry) + 1))

/* vpak mounts */

struct vpak_mount_content
{
  struct vpak_dir_entry* dir_entry;
  struct vpak_file_entry* file_entry;
  struct vpak_mount_content* next;
};

struct vpak_mount_entry
{
  wchar_t* name;
  int namesize;
  struct vpak_mount_entry* next;
  struct vpak_mount_entry* child;
  struct vpak_mount_content* content;
};

/**
 * vpak_find_by_name
 *
 * returns a child entry with the name specified
 * NULL if not found
 */
extern
struct vpak_entry*
vpak_find_by_name(struct vpak_dir_entry* parent,
                  const wchar_t* name,
                  int namesize);

/**
 * vpak_peek_component
 *
 * peeks a single component from the path
 * for UNC hostnames (e.g. \\host and \\?\UNC\host) returns host
 * for drives (e.g. C:\ and \\?\C:\) returns C:
 * returns pointer in path to the current component
 * updates path to point past the component
 * sets namesize to the size of component
 * returns NULL if path is empty
 */
extern
const wchar_t*
vpak_peek_component(const wchar_t** path,
                    int* namesize);

/**
 * vpak_add_mount
 *
 * mounts specified dir and file entries under the path specified
 * path must be absolute and normalized (e.g. GetFullPathName)
 * path can start with \\?\C:\name which will transform to C:\name
 * path can start with \\?\UNC\host\name which will transform to \\host\name
 */
extern
void
vpak_add_mount(const wchar_t* path,
               struct vpak_dir_entry* dir_entry,
               struct vpak_file_entry* file_entry);

/**
 * vpak_find_mount
 *
 * searches for appropriate mount point for path and returns its content
 * path must have same format as vpak_add_mount
 * rest will be point to the part of path that is under the mount point
 * caller is responsible for traversing returned linked list
 * this function does not search for files, only mount points
 */
extern
struct vpak_mount_content*
vpak_find_mount(const wchar_t* path,
                const wchar_t** rest);

#endif
