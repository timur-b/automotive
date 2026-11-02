#pragma once

#include <cstdint>

namespace input {

class Debouncer {
public:
    explicit Debouncer(uint32_t debounceMs = 30);

    void reset(bool initialState, uint32_t now);
    bool update(bool rawState, uint32_t now);
    bool state() const;

private:
    uint32_t debounceMs_;

    bool rawState_ = false;
    bool stableState_ = false;

    uint32_t rawChangedAt_ = 0;
};

} // namespace input
