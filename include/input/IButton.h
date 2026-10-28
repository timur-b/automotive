#pragma once

#include <cstdint>

namespace input {

class IButton {
public:
    virtual ~IButton() = default;

    virtual bool begin() = 0;
    virtual bool end() = 0;
    
    virtual bool isPressed() const = 0;
};

} // namespace input
