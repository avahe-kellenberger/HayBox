#include <fstream>
#include "modes/UltimateR4.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

std::ifstream f("profile.json");
json profile = json::parse(f);
json inputMap = profile["inputMap"];

#define ANALOG_STICK_MIN 28
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 228

#define pressed(b) (this->isPressed(inputs, b - 1))

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
}

void UltimateR4::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) { }

bool UltimateR4::isPressed(InputState &inputs, int buttonIndex) {
  switch (buttonIndex) {
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

void UltimateR4::assignValue(OutputState &outputs, std::string key, json value) {
  if (key == "a") {
    outputs.a = value;
  } else if (key == "b") {
    outputs.b = value;
  } else if (key == "x") {
    outputs.x = value;
  } else if (key == "y") {
    outputs.y = value;
  } else if (key == "zl") {
    outputs.buttonL = value;
  } else if (key == "zr") {
    outputs.buttonR = value;
  } else if (key == "l") {
    outputs.triggerLDigital = value;
  } else if (key == "r") {
    outputs.triggerRDigital = value;
  } else if (key == "start") {
    outputs.start = value;
  } else if (key == "select") {
    outputs.select = value;
  } else if (key == "home") {
    outputs.home = value;
  } else if (key == "dpadUp") {
    outputs.dpadUp = value;
  } else if (key == "dpadDown") {
    outputs.dpadDown = value;
  } else if (key == "dpadLeft") {
    outputs.dpadLeft = value;
  } else if (key == "dpadRight") {
    outputs.dpadRight = value;
  } else if (key == "leftStickClick") {
    outputs.leftStickClick = value;
  } else if (key == "rightStickClick") {
    outputs.rightStickClick = value;
  } else if (key == "leftStickX") {
    outputs.leftStickX = value;
  } else if (key == "leftStickY") {
    outputs.leftStickY = value;
   } else if (key == "rightStickX") {
    outputs.rightStickX = value;
  } else if (key == "rightStickY") {
    outputs.rightStickY = value;
  } else if (key == "triggerLAnalog") {
    outputs.triggerLAnalog = value;
  } else if (key == "triggerRAnalog") {
    outputs.triggerRAnalog = value;
  }
}

void UltimateR4::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
  for (json::iterator it = inputMap.begin(); it != inputMap.end(); it++) {
    json input = it.value();
    json buttons = input["buttons"];
    for (json::iterator btnIter = buttons.begin(); btnIter != buttons.end(); btnIter++) {
      int btn = btnIter.value();
      if (!pressed(btn)) {
        // Not all required buttons have been pressed, continue to the next inputMap element.
        goto ctn;
      }
    }

    {
      // Input match! Use input["output"] to set the output values.
      json output = input["output"];
      for (auto& [key, value] : output.items()) {
        assignValue(outputs, key, value);
      }
    }

    ctn:;
  }
}

