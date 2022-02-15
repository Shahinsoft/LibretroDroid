/*
 *     Copyright (C) 2019  Filippo Scognamiglio
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "input.h"

#include <cmath>

#include <android/input.h>
#include <android/keycodes.h>

#include "../../libretro-common/include/libretro.h"

namespace libretrodroid {

int16_t Input::getInputState(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (port >= 4 || port < 0) return 0;

    switch (device) {
        case RETRO_DEVICE_JOYPAD: {
            switch (id) {
                case RETRO_DEVICE_ID_JOYPAD_LEFT:
                    return pads[port].dpadXAxis == -1;
                case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                    return pads[port].dpadXAxis == 1;
                case RETRO_DEVICE_ID_JOYPAD_UP:
                    return pads[port].dpadYAxis == -1;
                case RETRO_DEVICE_ID_JOYPAD_DOWN:
                    return pads[port].dpadYAxis == 1;
                default:
                    return pads[port].pressedKeys.count(id) > 0;
            }
        }

        case RETRO_DEVICE_ANALOG: {
            switch (index) {
                case RETRO_DEVICE_INDEX_ANALOG_LEFT:
                    switch (id) {
                        case RETRO_DEVICE_ID_ANALOG_X:
                            return (int16_t) (pads[port].joypadLeftXAxis * MAX_RANGE_MOTION);
                        case RETRO_DEVICE_ID_ANALOG_Y:
                            return (int16_t) (pads[port].joypadLeftYAxis * MAX_RANGE_MOTION);
                        default:
                            return 0;
                    }
                case RETRO_DEVICE_INDEX_ANALOG_RIGHT:
                    switch (id) {
                        case RETRO_DEVICE_ID_ANALOG_X:
                            return (int16_t) (pads[port].joypadRightXAxis * MAX_RANGE_MOTION);
                        case RETRO_DEVICE_ID_ANALOG_Y:
                            return (int16_t) (pads[port].joypadRightYAxis * MAX_RANGE_MOTION);
                        default:
                            return 0;
                    }
                default:
                    return 0;
            }
        }

        case RETRO_DEVICE_POINTER: {
            // TODO... Here we should hanlde multitouch...
            if (index > 0) {
                return 0;
            }

            switch (id) {
                case RETRO_DEVICE_ID_POINTER_PRESSED: {
                    bool isXActive = pads[port].pointerScreenXAxis >= 0;
                    bool isYActive = pads[port].pointerScreenYAxis >= 0;
                    return (int16_t) (isXActive && isYActive ? 1 : 0);
                }

                case RETRO_DEVICE_ID_POINTER_X:
                    return (int16_t) (2.0 * (pads[port].pointerScreenXAxis - 0.5f) * MAX_RANGE_MOTION);

                case RETRO_DEVICE_ID_POINTER_Y:
                    return (int16_t) (2.0 * (pads[port].pointerScreenYAxis - 0.5f) * MAX_RANGE_MOTION);

                default:
                    return 0;
            }
        }

        default:
            return 0;
    }
}

int Input::convertAndroidToLibretroKey(int keyCode) const {
    switch (keyCode) {
        case AKEYCODE_BUTTON_START:
            return RETRO_DEVICE_ID_JOYPAD_START;
        case AKEYCODE_BUTTON_SELECT:
            return RETRO_DEVICE_ID_JOYPAD_SELECT;
        case AKEYCODE_BUTTON_A:
            return RETRO_DEVICE_ID_JOYPAD_A;
        case AKEYCODE_BUTTON_X:
            return RETRO_DEVICE_ID_JOYPAD_X;
        case AKEYCODE_BUTTON_Y:
            return RETRO_DEVICE_ID_JOYPAD_Y;
        case AKEYCODE_BUTTON_B:
            return RETRO_DEVICE_ID_JOYPAD_B;
        case AKEYCODE_BUTTON_L1:
            return RETRO_DEVICE_ID_JOYPAD_L;
        case AKEYCODE_BUTTON_L2:
            return RETRO_DEVICE_ID_JOYPAD_L2;
        case AKEYCODE_BUTTON_R1:
            return RETRO_DEVICE_ID_JOYPAD_R;
        case AKEYCODE_BUTTON_R2:
            return RETRO_DEVICE_ID_JOYPAD_R2;
        case AKEYCODE_BUTTON_THUMBL:
            return RETRO_DEVICE_ID_JOYPAD_L3;
        case AKEYCODE_BUTTON_THUMBR:
            return RETRO_DEVICE_ID_JOYPAD_R3;
        default:
            return UNKNOWN_KEY;
    }
}

void Input::onKeyEvent(unsigned int port, int action, int keyCode) {
    int retroKeyCode = convertAndroidToLibretroKey(keyCode);
    if (retroKeyCode == UNKNOWN_KEY) {
        return;
    }

    if (action == AKEY_EVENT_ACTION_DOWN) {
        pads[port].pressedKeys.insert(retroKeyCode);
    } else if (action == AKEY_EVENT_ACTION_UP) {
        pads[port].pressedKeys.erase(retroKeyCode);
    }
}

void Input::onMotionEvent(int port, int motionSource, float xAxis, float yAxis) {
    switch (motionSource) {
        case Input::MOTION_SOURCE_DPAD:
            pads[port].dpadXAxis = (int) round(xAxis);
            pads[port].dpadYAxis = (int) round(yAxis);
            break;

        case Input::MOTION_SOURCE_ANALOG_LEFT:
            pads[port].joypadLeftXAxis = xAxis;
            pads[port].joypadLeftYAxis = yAxis;
            break;

        case Input::MOTION_SOURCE_ANALOG_RIGHT:
            pads[port].joypadRightXAxis = xAxis;
            pads[port].joypadRightYAxis = yAxis;
            break;

        case Input::MOTION_SOURCE_POINTER:
            pads[port].pointerScreenXAxis = xAxis;
            pads[port].pointerScreenYAxis = yAxis;
            break;
    }
}

} //namespace libretrodroid
