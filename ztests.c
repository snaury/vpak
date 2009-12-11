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

#define TEST_PATHNAMES

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
