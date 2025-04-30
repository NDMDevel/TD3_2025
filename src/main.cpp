#include <Arduino.h>
#include "mb_slave.h"

MBSlave slave;

void setup()
{
  //PHY Config
  slave.setBaudrate(9600);
  slave.setParity(MBSlave::Parity::none);
  slave.setStopBits(MBSlave::StopBit::one);

  slave.serialInit();

  //MB Config
  slave.setID(25);

  slave.start();
}

void loop()
{
  slave.task();
}
