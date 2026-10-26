#pragma once

#include <cstdint>

class QuadratureDecoder {
public:
    void reset(bool a, bool b);
    int8_t update(bool a, bool b);

private:
    static constexpr int8_t TRANSITIONS[16] = {
         0, -1, +1,  0,
        +1,  0,  0, -1,
        -1,  0,  0, +1,
         0, +1, -1,  0
    };

    uint8_t previous_ = 0;
};
