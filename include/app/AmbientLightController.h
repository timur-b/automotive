#pragma once

#include "input/ILightSensor.h"
#include <cstdint>

namespace app {

class AmbientLightController {
public:
    struct Config {
        uint32_t sampleIntervalMs = 100;
        uint16_t darkThreshold = 1500;
        uint16_t brightThreshold = 2000;
        uint8_t filterShift = 3;
        bool darkerMeansLowerAdc = true;
    };

    AmbientLightController(input::ILightSensor& sensor, Config config);

    void begin(uint32_t now);
    void update(uint32_t now);

    bool isDark() const;
    uint16_t filteredValue() const;

private:
    input::ILightSensor& sensor_;
    Config config_;

    uint32_t lastSampleAt_ = 0;
    uint32_t filteredScaled_ = 0;

    bool initialized_ = false;
    bool dark_ = false;
};

} // namespace app
