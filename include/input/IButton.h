#pragma once

#include <cstdint>

namespace input {

class IButton {
public:
    virtual ~IButton() = default;

    virtual void begin() = 0;
    virtual bool isPressed() const = 0;
};

} // namespace input
