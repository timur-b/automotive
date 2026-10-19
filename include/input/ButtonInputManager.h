#pragma once

#include <cstddef>
#include <cstdint>

#include "input/IButtonInput.h"

namespace input {

class ButtonInputManager {
public:
    static constexpr size_t MAX_INPUTS = 8;

    bool add(IButtonInput& input);

    void begin(uint32_t now);
    void update(uint32_t now);

private:
    IButtonInput* inputs_[MAX_INPUTS] = {};
    size_t count_ = 0;
    bool initialized_ = false;
};

} // namespace input
