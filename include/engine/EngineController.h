#pragma once

#include <cstdint>

#include "domain/VehicleState.h"
#include "engine/IMotor.h"

namespace engine {

class EngineController {
public:
    explicit EngineController(IMotor& motor);

    void update(const domain::VehicleState& state);

    void reset();
    
private:
    IMotor& motor_;

    uint8_t previousPower_ = 0;
    bool firstUpdate_ = true;
};

} // namespace engine
