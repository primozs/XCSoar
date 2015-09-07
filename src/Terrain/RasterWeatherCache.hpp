/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2015 The XCSoar Project
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

#ifndef XCSOAR_TERRAIN_RASTER_WEATHER_HPP
#define XCSOAR_TERRAIN_RASTER_WEATHER_HPP

#include "Time/BrokenTime.hpp"
#include "Geo/GeoPoint.hpp"
#include "Compiler.h"

#include <tchar.h>

class RasterWeatherStore;
class RasterMap;
class OperationEnvironment;
struct zzip_dir;

/**
 * Class to manage the raster weather map, to be loaded/selected from
 * a #RasterWeatherStore instance.
 */
class RasterWeatherCache {
  const RasterWeatherStore &store;

  GeoPoint center = GeoPoint::Invalid();

  unsigned parameter = 0;
  unsigned last_parameter = 0;

  unsigned weather_time = 0;
  unsigned last_weather_time = 0;

  RasterMap *weather_map = nullptr;

public:
  /** 
   * Default constructor
   */
  explicit RasterWeatherCache(const RasterWeatherStore &_store)
    :store(_store) {}

  ~RasterWeatherCache() {
    Close();
  }

  const RasterWeatherStore &GetStore() const {
    return store;
  }

  void SetViewCenter(const GeoPoint &location, fixed radius);

  /**
   * Wrapper for RasterMap::IsDirty().
   */
  gcc_pure
  bool IsDirty() const;

  gcc_pure
  const RasterMap *GetMap() const {
    return weather_map;
  }

  /**
   * Are we currently displaying terrain instead of a RASP weather
   * map?
   */
  bool IsTerrain() const {
    return parameter == 0;
  }

  /**
   * Returns the current map's name.
   */
  gcc_pure
  const TCHAR *GetMapName() const;

  /**
   * Returns the index of the weather map being displayed.
   */
  gcc_pure
  unsigned GetParameter() const {
    return parameter;
  }

  /**
   * Switches to another weather map.
   */
  void SetParameter(unsigned i) {
    parameter = i;
  }

  /**
   * @param day_time the local time, in seconds since midnight
   */
  void Reload(BrokenTime time_local, OperationEnvironment &operation);

  /**
   * Returns the current time index.
   */
  gcc_pure
  BrokenTime GetTime() const;

  /**
   * Sets the current time index.
   */
  void SetTime(BrokenTime t);

private:
  void Close();
};

#endif
