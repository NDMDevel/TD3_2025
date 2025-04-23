#include "mb_slave.h"
#include <Arduino.h>
#include "TimerImp.hpp"

void MBSlave::setBaudrate(uint32_t bauds)
{
    _bauds = bauds;
}
void MBSlave::setParity(Parity parity)
{
    _parity = parity;
}
void MBSlave::setStopBits(StopBit stopBits)
{
    _stopBits = stopBits;
}
void MBSlave::serialInit()
{
    if( _parity == Parity::none && _stopBits == StopBit::one )
        Serial.begin(_bauds,SERIAL_8N1);
    if( _parity == Parity::even && _stopBits == StopBit::one )
        Serial.begin(_bauds,SERIAL_8E1);
    if( _parity == Parity::odd && _stopBits == StopBit::one )
        Serial.begin(_bauds,SERIAL_8O1);

    if( _parity == Parity::none && _stopBits == StopBit::two )
        Serial.begin(_bauds,SERIAL_8N2);
    if( _parity == Parity::even && _stopBits == StopBit::two )
        Serial.begin(_bauds,SERIAL_8E2);
    if( _parity == Parity::odd && _stopBits == StopBit::two )
        Serial.begin(_bauds,SERIAL_8O2);
}
void MBSlave::setID(uint8_t ID)
{

}
void MBSlave::task()
{
    if( _st == State::shutdown )
        return;
    if( _st == State::waitIdle )
    {
        //Serial.available();
        //Serial.read();
        //Serial.write(byte);
        Tim32_us tim;
        tim.start();


        while( Serial.available() )
            Serial.read();

        if( Serial.available() == 0 && tim > 100ms )
        {
            //bus serial in idle state...
        }

        if( tim < 100us )
            return;
        //do something...
        return;
    }
}
