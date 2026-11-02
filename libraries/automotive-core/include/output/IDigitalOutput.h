#pragma once

namespace output {

    class IDigitalOutput {
public:
    virtual ~IDigitalOutput() = default;

    virtual bool begin() = 0;
    virtual bool end() = 0;

    virtual void set(bool active) = 0;
};

} // namespace output
