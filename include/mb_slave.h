#pragma once

#include <stdint.h>

class MBSlave
{
public:
    enum class Parity
    {
        none,
        even,
        odd
    };
    enum class StopBit
    {
        one,
        two
    };
private:
    enum class State
    {
        shutdown,
        waitIdle
    };
private:
    uint32_t _bauds         = 9600;
    Parity   _parity        = Parity::none;
    StopBit  _stopBits      = StopBit::one;
    State    _st            = State::shutdown;
public:
    void setBaudrate(uint32_t bauds);
    void setParity(Parity parity);
    void setStopBits(StopBit stopBits);

    void serialInit();

    void setID(uint8_t ID);
    void task();
};
