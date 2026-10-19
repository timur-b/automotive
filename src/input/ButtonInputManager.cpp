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

void ButtonInputManager::begin(uint32_t now) {
    if (initialized_) {
        return;
    }

    for (size_t i = 0; i < count_; ++i) {
        inputs_[i]->begin(now);
    }

    initialized_ = true;
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
