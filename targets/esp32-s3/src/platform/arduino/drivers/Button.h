#pragma once

#include "input/IButton.h"
#include <cstdint>

namespace drivers {

enum class ButtonPolarity { ACTIVE_LOW, ACTIVE_HIGH };

class Button final : public input::IButton {
public:
    Button(uint8_t pin, ButtonPolarity polarity);

    bool begin() override;
    bool end() override;

    bool isPressed() const override;

private:
    uint8_t pin_;
    ButtonPolarity polarity_;

    bool initialized_ = false;
};

} // namespace drivers
