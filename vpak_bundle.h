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

#define VPAK_BUNDLE_TYPE_DIR 0
#define VPAK_BUNDLE_TYPE_FILE 1

/* bundle file format */

struct vpak_bundle_entry
{
  int type;
  char* name;
  wchar_t* wname;
  /* TODO: attributes, creation/modification time */
  unsigned char data[1]; /* dir_entry/file_entry/etc */
};

struct vpak_bundle_dir_entry
{
  unsigned int nchildren;
  struct vpak_bundle_entry* children[1];
};

struct vpak_bundle_file_entry
{
  unsigned int size;
  unsigned char* data;
};

struct vpak_bundle_mount_entry
{
  char* name;
  wchar_t* wname;
  struct vpak_bundle_dir_entry* content;
};

struct vpak_bundle_header
{
  struct vpak_bundle_file_entry* emulation;
  struct vpak_bundle_dir_entry* content;
  unsigned int nmounts;
  struct vpak_bundle_mount_entry* mounts[1];
};

extern struct vpak_bundle_entry* vpak_find_by_name(struct vpak_bundle_dir_entry* parent, const char* name);
extern struct vpak_bundle_entry* vpak_find_by_wname(struct vpak_bundle_dir_entry* parent, const wchar_t* wname);

/* vpak mounts */

struct vpak_mount_content
{
  struct vpak_bundle_dir_entry* dir_entry;
  struct vpak_bundle_file_entru* file_entry;
  struct vpak_bundle_mount_content* next;
};

struct vpak_mount_entry
{
  char* name;
  int namesize;
  wchar_t* wname;
  int wnamesize;
  struct vpak_mount_entry* next;
  struct vpak_mount_entry* child;
  struct vpak_mount_content* content;
};

extern void vpak_add_mount(const char* name,
                           struct vpak_bundle_dir_entry* dir_entry,
                           struct vpak_bundle_file_entry* file_entry);
extern void vpak_add_wmount(const wchar_t* wname,
                            struct vpak_bundle_dir_entry* dir_entry,
                            struct vpak_bundle_file_entry* file_entry);
extern struct vpak_mount_content* vpak_find_mount(const char* name, const char** rest);
extern struct vpak_mount_content* vpak_find_wmount(const wchar_t* wname, const char** wrest);
