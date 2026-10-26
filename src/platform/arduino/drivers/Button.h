#pragma once

#include "input/IButton.h"
#include <cstdint>

namespace drivers {

enum class ButtonPolarity { ACTIVE_LOW, ACTIVE_HIGH };

class Button final : public input::IButton {
public:
    Button(uint8_t pin, ButtonPolarity polarity);

    void begin() override;
    bool isPressed() const override;

private:
    uint8_t pin_;
    ButtonPolarity polarity_;
};

} // namespace drivers
