#ifndef _MODES_ULTIMATE_R4_HPP
#define _MODES_ULTIMATE_R4_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class UltimateR4 : public ControllerMode {
  public:
    UltimateR4(socd::SocdType socd_type, char* rawJson);

  private:
    int _buttons[20];
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);
    bool isPressed(InputState &inputs, int defaultButtonIndex);
    void assignValue(OutputState &outputs, std::string key, nlohmann::json value);
};

#endif

