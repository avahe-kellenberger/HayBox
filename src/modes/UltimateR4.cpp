#include "modes/UltimateR4.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

json profile = json::parse(R"(
  {
    "name": "Default Profile",
    "socd": "2IP",
    "buttons": {
      "l": 1,
      "left": 2,
      "down": 3,
      "right": 4,
      "modx": 5,
      "mody": 6,
      "start": 7,
      "r": 8,
      "y": 9,
      "lightshield": 10,
      "midshield": 11,
      "b": 12,
      "x": 13,
      "zr": 14,
      "up": 15,
      "c-left": 16,
      "c-up": 17,
      "c-down": 18,
      "a": 19,
      "c-right": 20
    }
  }
)");


#define ANALOG_STICK_MIN 28
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 228

#define pressed(b) (this->isPressed(inputs, b))
#define profileBtn(b) (profile["buttons"][b].template get<int>() - 1)

/**
 * Default button indices.
 */
const int btnL = 0;
const int btnLeft = 1;
const int btnDown = 2;
const int btnRight = 3;
const int btnModX = 4;
const int btnModY = 5;
const int btnStart = 6;
const int btnR = 7;
const int btnY = 8;
const int btnLightshield = 9;
const int btnMidshield = 10;
const int btnB = 11;
const int btnX = 12;
const int btnZ = 13;
const int btnUp = 14;
const int btnCLeft = 15;
const int btnCUp = 16;
const int btnCDown = 17;
const int btnA = 18;
const int btnCRight = 19;

UltimateR4::UltimateR4(socd::SocdType socd_type) {
  _socd_pair_count = 4;
  _socd_pairs = new socd::SocdPair[_socd_pair_count]{
    socd::SocdPair{ &InputState::left,   &InputState::right, socd_type },
    socd::SocdPair{ &InputState::down,   &InputState::up, socd_type },
    socd::SocdPair{ &InputState::c_left, &InputState::c_right, socd_type },
    socd::SocdPair{ &InputState::c_down, &InputState::c_up, socd_type }
  };

  // Default button index mapping
  _buttons[btnL] = profileBtn("l");
  _buttons[btnLeft] = profileBtn("left");
  _buttons[btnDown] = profileBtn("down");
  _buttons[btnRight] = profileBtn("right");
  _buttons[btnModX] = profileBtn("modx");
  _buttons[btnModY] = profileBtn("mody");
  _buttons[btnStart] = profileBtn("start");
  _buttons[btnR] = profileBtn("r");
  _buttons[btnY] = profileBtn("y");
  _buttons[btnLightshield] = profileBtn("lightshield");
  _buttons[btnMidshield] = profileBtn("midshield");
  _buttons[btnB] = profileBtn("b");
  _buttons[btnX] = profileBtn("x");
  _buttons[btnZ] = profileBtn("zr");
  _buttons[btnUp] = profileBtn("up");
  _buttons[btnCLeft] = profileBtn("c-left");
  _buttons[btnCUp] = profileBtn("c-up");
  _buttons[btnCDown] = profileBtn("c-down");
  _buttons[btnA] = profileBtn("a");
  _buttons[btnCRight] = profileBtn("c-right");
}

void UltimateR4::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
    outputs.a = pressed(btnA);
    outputs.b = pressed(btnB);
    outputs.x = pressed(btnX);
    outputs.y = pressed(btnY);
    outputs.buttonL = pressed(btnMidshield);
    outputs.buttonR = pressed(btnZ);
    outputs.triggerLDigital = pressed(btnL);
    outputs.triggerRDigital = pressed(btnR);
    outputs.start = pressed(btnStart);

    // Turn on D-Pad layer by holding Mod X + Mod Y
    if (pressed(btnModX) && pressed(btnModY)) {
        outputs.dpadUp = pressed(btnCUp);
        outputs.dpadDown = pressed(btnCDown);
        outputs.dpadLeft = pressed(btnCLeft);
        outputs.dpadRight = pressed(btnCRight);
    }
}

bool UltimateR4::isPressed(InputState &inputs, int defaultButtonIndex) {
  int remappedButtonIndex = _buttons[defaultButtonIndex];
  switch (remappedButtonIndex) {
    case btnL:
      return inputs.l;
    case btnLeft:
      return inputs.left;
    case btnDown:
      return inputs.down;
    case btnRight:
      return inputs.right;
    case btnModX:
      return inputs.mod_x;
    case btnModY:
      return inputs.mod_y;
    case btnStart:
      return inputs.start;
    case btnR:
      return inputs.r;
    case btnY:
      return inputs.y;
    case btnLightshield:
      return inputs.lightshield;
    case btnMidshield:
      return inputs.midshield;
    case btnB:
      return inputs.b;
    case btnX:
      return inputs.x;
    case btnZ:
      return inputs.z;
    case btnUp:
      return inputs.up;
    case btnCLeft:
      return inputs.c_left;
    case btnCUp:
      return inputs.c_up;
    case btnCDown:
      return inputs.c_down;
    case btnA:
      return inputs.a;
    case btnCRight:
      return inputs.c_right;
  }
  return false;
}

void UltimateR4::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
    // Coordinate calculations to make modifier handling simpler.
    UpdateDirections(
        pressed(btnLeft),
        pressed(btnRight),
        pressed(btnDown),
        pressed(btnUp),
        pressed(btnCLeft),
        pressed(btnCRight),
        pressed(btnCDown),
        pressed(btnCUp),
        ANALOG_STICK_MIN,
        ANALOG_STICK_NEUTRAL,
        ANALOG_STICK_MAX,
        outputs
    );

    /**
     * If I want to check if "left" is pressed,
     * I need the button selected by the user that represents left.
     *
     * Say the user swaps L (index 0) and Left (index 1).
     * To check for "left",
     *
     * I would actually need to check index 0 instead of index 1.
     * This means I need to derive the "answer" 0 from 1, e.g.:
     *
     * int getButtonIndex(int defaultIndex) {
     *   return buttons[defaultIndex];
     * }
     *
     * Above, buttons is an int[] which contains the remapped button indices.
     * We can then do the below:
     * bool isLeftPressed(InputState &inputs) {
     *   return isPressed(buttons[1]);
     * }
     *
     * bool isRPressed(InputState &inputs) {
     *   return isPressed(buttons[8]);
     * }
     * and so on.
     *
     * We then need to implement the "isPressed" function.
     * This function takes in the default index of a button,
     * and returns the corresponding InputState value.
     * This could be a simple case statement with hard-coded "returns left", etc.
     */

    bool shield_button_pressed = pressed(btnL) || pressed(btnR);

    if (pressed(btnModX)) {
        if (directions.horizontal) {
          if (shield_button_pressed) {
            // MX + Horizontal = 51 for shield tilt
            outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 51);
          } else {
            // Fastest walking speed before run
            outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 53);
          }
        } else if (directions.vertical) {
            // Vertical Shield Tilt and crouch with mod_x = 65
            outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 65);
        } else if (directions.diagonal) {
            // MX + q1/2/3/4 = 53 35
            outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 53);
            outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 34);
            if (shield_button_pressed) {
                // TODO: If holding shield, then side, THEN down, it will spot dodge.
                // MX + L, R, LS, and MS + q1/2/3/4 = 6375 3750 = 51 30
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 51);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 30);
            }
        }

        // Angled fsmash/ftilt with C-Stick + MX
        if (directions.cx != 0) {
            outputs.rightStickX = ANALOG_STICK_NEUTRAL + (directions.cx * 100);
            outputs.rightStickY = ANALOG_STICK_NEUTRAL + (directions.y * 59);
        }

        /* Up B angles */
        if (directions.diagonal && !shield_button_pressed) {
            // (39.05) = 53 43
            if (pressed(btnCDown)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 53);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 43);
            }
            // (36.35) = 53 39
            if (pressed(btnCLeft)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 53);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 39);
            }
            // (30.32) = 56 41
            if (pressed(btnCUp)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 53);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 31);
            }
            // (27.85) = 49 42
            if (pressed(btnCRight)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 53);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 28);
            }

            /* Extended Up B Angles */
            if (pressed(btnB)) {
                // (33.29) = 67 44
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 67);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 44);
                // (39.38) = 67 55
                if (pressed(btnCDown)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 67);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 55);
                }
                // (36.18) = 67 49
                if (pressed(btnCLeft)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 67);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 49);
                }
                // (30.2) = 67 39
                if (pressed(btnCUp)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 67);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 39);
                }
                // (27.58) = 67 35
                if (pressed(btnCRight)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 67);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 35);
                }
            }

            // Angled Ftilts
            if (pressed(btnA)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 36);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 26);
            }
        }
    }

    if (pressed(btnModY)) {
        if (directions.horizontal) {
          if (shield_button_pressed) {
            outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 51);
          } else {
            // Allow tink/yink walk shield
            outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 28);
          }
        } else if (directions.vertical) {
            // Vertical Shield Tilt and crouch with mod_x = 50
            outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 50);
        } else if (directions.diagonal) {
            outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 53);
            outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 34);
            if (shield_button_pressed) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 51);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 30);
            }
        }

        /* Up B angles */
        if (directions.diagonal && !shield_button_pressed) {
            // (50.95) = 43 53
            if (pressed(btnCDown)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 43);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 53);
            }
            // (53.65) = 39 53
            if (pressed(btnCLeft)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 49);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 53);
            }
            // (59.68) = 31 53
            if (pressed(btnCUp)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 31);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 53);
            }
            // (62.15) = 28 53
            if (pressed(btnCRight)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 28);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 53);
            }

            /* Extended Up B Angles */
            if (pressed(btnB)) {
                // (56.71) = 44 67
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 44);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 67);
                // (50.62) = 55 67
                if (pressed(btnCDown)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 55);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 67);
                }
                // (53.82) = 49 67
                if (pressed(btnCLeft)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 49);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 67);
                }
                // (59.8) = 39 67
                if (pressed(btnCUp)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 39);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 67);
                }
                // (62.42) = 35 67
                if (pressed(btnCRight)) {
                    outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 35);
                    outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 67);
                }
            }

            // MY Pivot Uptilt/Dtilt
            if (pressed(btnA)) {
                outputs.leftStickX = ANALOG_STICK_NEUTRAL + (directions.x * 50);
                outputs.leftStickY = ANALOG_STICK_NEUTRAL + (directions.y * 65);
            }
        }
    }

    // C-stick ASDI Slideoff angle overrides any other C-stick modifiers (such as angled fsmash).
    if (directions.cx != 0 && directions.cy != 0) {
        // 5250 8500 = 42 68
        outputs.rightStickX = ANALOG_STICK_NEUTRAL + (directions.cx * 42);
        outputs.rightStickY = ANALOG_STICK_NEUTRAL + (directions.cy * 68);
    }

    if (pressed(btnL)) {
        outputs.triggerLAnalog = 140;
    }

    if (pressed(btnR)) {
        outputs.triggerRAnalog = 140;
    }

    // Shut off C-stick when using D-Pad layer.
    if (pressed(btnModX) && pressed(btnModY)) {
        outputs.rightStickX = ANALOG_STICK_NEUTRAL;
        outputs.rightStickY = ANALOG_STICK_NEUTRAL;
      
        if (pressed(btnLightshield)) {
            outputs.select = true;
        }

        if (pressed(btnMidshield)) {
            outputs.home = true;
        }

        if (pressed(btnStart)) {
            outputs.home = true;
            outputs.start = false;
        }
    }

}

