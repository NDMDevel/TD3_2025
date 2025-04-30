#include "mb_slave.h"
#include <Arduino.h>
#include "TimerImp.hpp"

unsigned int crc_16(unsigned char *buffer, unsigned int length)
{
    unsigned int i, j, temp_bit, temp_int, crc;
    crc = 0xFFFF;
    for (i = 0; i < length; i++)
    {
        temp_int = (unsigned char)*buffer++;
        crc ^= temp_int;
        for (j = 0; j < 8; j++)
        {
            temp_bit = crc & 0x0001;
            crc >>= 1;
            if (temp_bit != 0)
                crc ^= 0xA001;
        }
    }
    uint16_t inv_crc = crc >> 8;
    inv_crc = inv_crc | ((crc << 8)&0xFF00);
    return inv_crc;
}


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
void MBSlave::start()
{
    if( _st == State::shutdown )
        _st = State::resetTimer;
}
void MBSlave::task()
{
    if( _st == State::shutdown )
        return;
    if( _st == State::resetTimer )
    {
        //clean the rx fifo
        while( Serial.available() )
            Serial.read();

        _tim.start();
        _st = State::waitIdle;
        return;
    }
    if( _st == State::waitIdle )
    {
        uint8_t DataBits = 8;
        uint8_t ParityBits;
        if( _parity == Parity::none )
            ParityBits = 0;
        if( _parity == Parity::even || _parity == Parity::odd )
            ParityBits = 1;

        uint8_t StopBits;
        if( _stopBits == StopBit::one )
            StopBits = 1;
        if( _stopBits == StopBit::two )
            StopBits = 2;
        
        float tIdle = float(1 + DataBits + ParityBits + StopBits) / float(_bauds) * 4;
        if( Serial.available() == 0 )
        {
            if( _tim > 4167us /*tIdle*/ )
            {
                //bus serial in idle state...
                _st = State::ReadMBFrame;
                return;
            }
        }
        else
        {
            _st = State::resetTimer;
            return;
        }
        return;
    }
    if( _st == State::ReadMBFrame )
    {
        if( Serial.available() == 0 )
            return;
        //first MB Frame byte arrived
        _mb_frame_len = Serial.available();
        _tim.restart();
        _st = State::waitIdleEnd;
        return;
    }
    if( _st == State::waitIdleEnd )
    {
        if( Serial.available() != _mb_frame_len )
        {
            _mb_frame_len = Serial.available();
            _tim.restart();
            return;
        }
        else
        {
            if( _tim > 4167us )
            {
                //MB Frame finished
                uint8_t rx[300];    //more than 256
                Serial.read(rx,Serial.available());
                rx[0] = 0x0A;
                rx[1] = 0x03;
                rx[2] = 2;
                rx[3] = 0x03;
                rx[4] = 0xE8;
                uint16_t crc = crc_16(rx,5);
                rx[5] = crc >> 8;
                rx[6] = crc & 0x00FF;
                Serial.write(rx,7);

//                Serial.write(rx,_mb_frame_len);
                _st = State::resetTimer;
                return;
            }
        }
        return;
    }
}
