#pragma once

#include <stdint.h>
#include "TimerImp.hpp"

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
        resetTimer,
        waitIdle,
        ReadMBFrame,
        waitIdleEnd
    };
private:
    uint32_t _bauds         = 9600;
    Parity   _parity        = Parity::none;
    StopBit  _stopBits      = StopBit::one;
    State    _st            = State::shutdown;
    Tim32_us _tim;
    int _mb_frame_len;
public:
    void setBaudrate(uint32_t bauds);
    void setParity(Parity parity);
    void setStopBits(StopBit stopBits);

    void serialInit();

    void setID(uint8_t ID);
    void start();
    void task();
};
