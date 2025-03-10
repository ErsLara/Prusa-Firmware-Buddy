/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if HAS_PID_HEATING && ENABLED(PID_AUTOTUNE)

#include "../gcode.h"
#include "../../module/temperature.h"

/** \addtogroup G-Codes
 * @{
 */

/**
 *### M303: Run PID tuning <a href="https://reprap.org/wiki/G-code#M303:_Run_PID_tuning">M303: Run PID tuning</a>
 *
 *#### Usage
 *
 *    M303 [ S | E | C | U ]
 *
 *#### Parameters
 *
 * - `S` - Set the target temperature. (default 150C / 70C)
 * - `E` - Extruder
 *   - `-1` - for the bed
 * - `C` - Cycles minimum 3. Default 5.
 * - `U` - Use PID result
 */
void GcodeSuite::M303() {
  #if ENABLED(PIDTEMPBED)
    #define SI H_BED
  #else
    #define SI H_E0
  #endif
  #if ENABLED(PIDTEMP)
    #define EI HOTENDS - 1
  #else
    #define EI H_BED
  #endif
  const heater_ind_t e = (heater_ind_t)parser.intval('E');
  if (!WITHIN(e, SI, EI)) {
    SERIAL_ECHOLNPGM(MSG_PID_BAD_EXTRUDER_NUM);
    return;
  }

  const int c = parser.intval('C', 5);
  const bool u = parser.boolval('U');
  const int16_t temp = parser.celsiusval('S', e < 0 ? 70 : 150);

  #if DISABLED(BUSY_WHILE_HEATING)
    KEEPALIVE_STATE(NOT_BUSY);
  #endif

  thermalManager.PID_autotune(temp, e, c, u);
}

/** @}*/

#endif // HAS_PID_HEATING
