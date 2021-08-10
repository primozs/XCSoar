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

#include "BrokenDateTime.hpp"
#include "Calendar.hxx"
#include "Convert.hxx"

#include <cassert>
#include <time.h>

#ifndef HAVE_POSIX
#include <sysinfoapi.h>
#include <timezoneapi.h>
#endif

#ifdef HAVE_POSIX

static const BrokenDateTime
ToBrokenDateTime(const struct tm &tm) noexcept
{
  BrokenDateTime dt;

  dt.year = tm.tm_year + 1900;
  dt.month = tm.tm_mon + 1;
  dt.day = tm.tm_mday;
  dt.day_of_week = tm.tm_wday;

  dt.hour = tm.tm_hour;
  dt.minute = tm.tm_min;
  dt.second = tm.tm_sec;

  return dt;
}

BrokenDateTime
BrokenDateTime::FromUnixTimeUTC(int64_t t) noexcept
{
  return ToBrokenDateTime(GmTime(std::chrono::system_clock::from_time_t(t)));
}

#else /* !HAVE_POSIX */

static const BrokenDateTime
ToBrokenDateTime(const SYSTEMTIME st) noexcept
{
  BrokenDateTime dt;

  dt.year = st.wYear;
  dt.month = st.wMonth;
  dt.day = st.wDay;
  dt.day_of_week = st.wDayOfWeek;

  dt.hour = st.wHour;
  dt.minute = st.wMinute;
  dt.second = st.wSecond;

  return dt;
}

static const BrokenDateTime
ToBrokenDateTime(const FILETIME &ft) noexcept
{
  SYSTEMTIME st;
  FileTimeToSystemTime(&ft, &st);
  return ToBrokenDateTime(st);
}

static const SYSTEMTIME
ToSystemTime(const BrokenDateTime &dt) noexcept
{
  SYSTEMTIME st;

  st.wYear = dt.year;
  st.wMonth = dt.month;
  st.wDay = dt.day;
  st.wDayOfWeek = dt.day_of_week;

  st.wHour = dt.hour;
  st.wMinute = dt.minute;
  st.wSecond = dt.second;
  st.wMilliseconds = 0;

  return st;
}

static const FILETIME
ToFileTime(const BrokenDateTime &dt) noexcept
{
  SYSTEMTIME st = ToSystemTime(dt);
  FILETIME ft;
  SystemTimeToFileTime(&st, &ft);
  return ft;
}

static time_t
timegm (struct tm *tm) noexcept
{
  static constexpr unsigned ndays[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
  };

  time_t res = 0;

  for (int year = 70; year < tm->tm_year; ++year)
    res += IsLeapYear(year) ? 366 : 365;

  for (int month = 0; month < tm->tm_mon; ++month)
    res += ndays[month];

  if (tm->tm_mon > 1 && IsLeapYear(tm->tm_year + 1900))
    res++;

  res += tm->tm_mday - 1;
  res *= 24;
  res += tm->tm_hour;
  res *= 60;
  res += tm->tm_min;
  res *= 60;
  res += tm->tm_sec;
  return res;
}
#endif

int64_t
BrokenDateTime::ToUnixTimeUTC() const noexcept
{
  assert(IsPlausible());

  struct tm tm;
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;
  tm.tm_isdst = 0;
  return ::timegm(&tm);
}

const BrokenDateTime
BrokenDateTime::NowUTC() noexcept
{
#ifdef HAVE_POSIX
  time_t t = time(NULL);
  return FromUnixTimeUTC(t);
#else /* !HAVE_POSIX */
  SYSTEMTIME st;
  GetSystemTime(&st);

  return ToBrokenDateTime(st);
#endif /* !HAVE_POSIX */
}

const BrokenDateTime
BrokenDateTime::NowLocal() noexcept
{
#ifdef HAVE_POSIX
  time_t t = time(NULL);
  struct tm tm;
  localtime_r(&t, &tm);

  return ToBrokenDateTime(tm);
#else /* !HAVE_POSIX */
  SYSTEMTIME st;
  GetLocalTime(&st);

  return ToBrokenDateTime(st);
#endif /* !HAVE_POSIX */
}

BrokenDateTime
BrokenDateTime::operator+(int seconds) const noexcept
{
  assert(IsPlausible());

#ifdef HAVE_POSIX
  return FromUnixTimeUTC(ToUnixTimeUTC() + seconds);
#else
  FILETIME ft = ToFileTime(*this);
  ULARGE_INTEGER uli;
  uli.u.HighPart = ft.dwHighDateTime;
  uli.u.LowPart = ft.dwLowDateTime;
  uli.QuadPart += (LONGLONG)seconds * 10000000;
  ft.dwHighDateTime = uli.u.HighPart;
  ft.dwLowDateTime = uli.u.LowPart;
  return ToBrokenDateTime(ft);
#endif
}

int
BrokenDateTime::operator-(const BrokenDateTime &other) const noexcept
{
  return ToUnixTimeUTC() - other.ToUnixTimeUTC();
}
