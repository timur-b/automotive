#pragma once

#include "input/ButtonEvents.h"
#include <cstdint>

namespace input {

class IButtonInput {
public:
    using EventCallback = void (*)(ButtonEdge edge, ButtonGesture gesture, void* context);

    virtual ~IButtonInput() = default;

    virtual bool begin(uint32_t now) = 0;
    virtual bool end() = 0;

    virtual void update(uint32_t now) = 0;

    virtual void setCallback(EventCallback callback, void* context = nullptr) = 0;

    virtual bool isPressed() const = 0;
};

} // namespace input
