#include "input/Debouncer.h"

namespace input {

Debouncer::Debouncer(uint32_t debounceMs) : debounceMs_(debounceMs) {}

void Debouncer::reset(bool initialState, uint32_t now) {
    rawState_ = initialState;
    stableState_ = initialState;
    rawChangedAt_ = now;
}

bool Debouncer::update(bool rawState, uint32_t now) {
    if (rawState != rawState_) {
        rawState_ = rawState;
        rawChangedAt_ = now;
    }

    if (rawState_ == stableState_) {
        return false;
    }

    if (static_cast<uint32_t>(now - rawChangedAt_) < debounceMs_) {
        return false;
    }

    stableState_ = rawState_;
    return true;
}

bool Debouncer::state() const {
    return stableState_;
}

} // namespace input
