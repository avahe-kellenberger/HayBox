#ifndef _CORE_COMMUNICATIONBACKEND_HPP
#define _CORE_COMMUNICATIONBACKEND_HPP

#include "core/ControllerMode.hpp"
#include "core/InputSource.hpp"
#include "state.hpp"

#include <chrono>

#define ANALOG_STICK_NEUTRAL 128
const float FRAME_DURATION = 1.0 / 60.0;

class CommunicationBackend {
  public:
    CommunicationBackend(InputSource **input_sources, size_t input_source_count);
    virtual ~CommunicationBackend(){};

    InputState &GetInputs();
    void ScanInputs();
    void ScanInputs(InputScanSpeed input_source_filter);

    void UpdateOutputs();
    virtual void SetGameMode(ControllerMode *gamemode);

    virtual void SendReport() = 0;

  protected:
    InputState _inputs;
    InputSource **_input_sources;
    size_t _input_source_count;

    OutputState _outputs;
    ControllerMode *_gamemode;

    bool _isRightStickNeutral;
    std::chrono::time_point<std::chrono::high_resolution_clock> _timerStart;

  private:
    void ResetOutputs();
};

#endif
