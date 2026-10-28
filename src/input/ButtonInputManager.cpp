#include "input/ButtonInputManager.h"

namespace input {

bool ButtonInputManager::add(IButtonInput& input) {
    if (initialized_ || count_ >= MAX_INPUTS) {
        return false;
    }

    // Prevent registering the same input twice.
    for (size_t i = 0; i < count_; ++i) {
        if (inputs_[i] == &input) {
            return false;
        }
    }

    inputs_[count_++] = &input;

    return true;
}

bool ButtonInputManager::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    for (size_t i = 0; i < count_; ++i) {
        if (!inputs_[i]->begin(now)) {
            return false;
        }
    }

    initialized_ = true;
    return true;
}

bool ButtonInputManager::end() {
    bool success = true;

    // Disable input processing before releasing resources.
    initialized_ = false;

    // Stop every registered input, even if one fails.
    for (size_t i = 0; i < count_; ++i) {
        if (!inputs_[i]->end()) {
            success = false;
        }
    }

    if (!success) {
        return false;
    }

    // Clear registrations only after successful cleanup.
    for (size_t i = 0; i < count_; ++i) {
        inputs_[i] = nullptr;
    }

    count_ = 0;
    initialized_ = false;

    return true;
}

void ButtonInputManager::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    for (size_t i = 0; i < count_; ++i) {
        inputs_[i]->update(now);
    }
}

} // namespace input
