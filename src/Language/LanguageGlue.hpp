/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2021 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#pragma once

void
InitLanguage() noexcept;

void
ReadLanguageFile() noexcept;

void
CloseLanguageFile() noexcept;

#if defined(HAVE_POSIX) && !defined(ANDROID) && !defined(KOBO) && !defined(__APPLE__)

/**
 * Using the C library's gettext implementation instead of rolling our
 * own.
 */
#define HAVE_NATIVE_GETTEXT

#else

#define HAVE_BUILTIN_LANGUAGES

#include <cstddef>
#include <tchar.h>

struct BuiltinLanguage {
#ifdef _WIN32
  unsigned language;
#endif
  const void *begin;
  size_t size;
  const TCHAR *resource;
  const TCHAR *name;
};

extern const BuiltinLanguage language_table[];

#endif
