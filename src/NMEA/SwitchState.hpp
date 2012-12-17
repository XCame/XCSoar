/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
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

#ifndef XCSOAR_SWITCH_STATE_HPP
#define XCSOAR_SWITCH_STATE_HPP

#include <stdint.h>

/**
 * State of external switch devices (esp Vega)
 */
struct SwitchState
{
  enum class FlightMode: uint8_t {
    UNKNOWN,
    CIRCLING,
    CRUISE,
  };

  enum class FlapPosition : uint8_t {
    UNKNOWN,
    POSITIVE,
    NEUTRAL,
    NEGATIVE,
    LANDING,
  };

  enum class UserSwitch : uint8_t {
    UNKNOWN,
    UP,
    MIDDLE,
    DOWN,
  };

  enum class AirbrakeState : uint8_t {
    UNKNOWN,
    LOCKED,
    NOT_LOCKED,
  };

  FlightMode flight_mode;
  FlapPosition flap_position;
  UserSwitch user_switch;
  AirbrakeState airbrake_state;

  bool gear_extended;
  bool acknowledge;
  bool repeat;
  bool speed_command;
  // bool stall;

  void Reset();
};

#endif