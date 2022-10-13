//  Author: avishorp@gmail.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef __TM1637DISPLAY__
#define __TM1637DISPLAY__

#include <inttypes.h>

#define SEG_A 0b00000001
#define SEG_B 0b00000010
#define SEG_C 0b00000100
#define SEG_D 0b00001000
#define SEG_E 0b00010000
#define SEG_F 0b00100000
#define SEG_G 0b01000000

//  Author: avishorp@gmail.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// #include <stdlib.h>
// #include <string.h>
// #include <inttypes.h>

#include <TM1637Display.h>

#define TM1637_I2C_COMM1 0x40
#define TM1637_I2C_COMM2 0xC0
#define TM1637_I2C_COMM3 0x80

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D
const uint8_t digitToSegment[] = {
    // XGFEDCBA
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // B
    0b00111001, // C
    0b01000111, // D
    0b01111001, // E
    0b01110001  // F
};
void (*SetClockDir)(uint8_t dir);
void (*SetClock)(uint8_t value);
void (*SetDataDir)(uint8_t dir);
void (*SetData)(uint8_t value);
int (*ReadData)(void);
void (*Delay)(uint32_t us);

uint8_t m_brightness;
//======================================================================================================

void TM1637Display(void (*SetClockDirCallback)(uint8_t),
                   void (*SetClockCallback)(uint8_t),
                   void (*SetDataDirCallback)(uint8_t),
                   void (*SetDataCallback)(uint8_t),
                   int (*ReadDataCallback)(void),
                   void (*DelayCallback)(uint32_t))
{
  // Copy the pin numbers
  SetClock = SetClockCallback;
  SetData = SetDataCallback;
  SetDataDir = SetDataDirCallback;
  SetClockDir = SetClockDirCallback;
  ReadData = ReadDataCallback;
  Delay = DelayCallback;

  // Set the pin direction and default value.
  // Both pins are set as inputs, allowing the pull-up resistors to pull them up
  SetClockDir(0);
  SetDataDir(0);
  SetClock(0);
  SetData(0);
}
//======================================================================================================

void setBrightness(uint8_t brightness)
{
  m_brightness = brightness;
}
//======================================================================================================

void bitDelay()
{

  Delay(50);
}
//======================================================================================================

void start()
{
 
  SetDataDir(1);
  bitDelay();
}
//======================================================================================================

void stop()
{

  SetDataDir(1);
  bitDelay();
  SetClockDir(0);
  bitDelay();
  SetDataDir(0);
  bitDelay();
}
//======================================================================================================

bool writeByte(uint8_t b)
{
  uint8_t data = b;

  // 8 Data Bits
  for (uint8_t i = 0; i < 8; i++)
  {
    // CLK low
    SetClockDir(1);
    bitDelay();

    // Set data bit
    if (data & 0x01)
      SetDataDir(0);
    else
      SetDataDir(1);

    bitDelay();

    // CLK high
    SetClockDir(0);
    bitDelay();
    data = data >> 1;
  }

  // Wait for acknowledge
  // CLK to zero
  SetClockDir(1);
  SetDataDir(0);
  bitDelay();

  // CLK to high
  SetClockDir(0);
  bitDelay();
  uint8_t ack = ReadData();
  if (ack == 0)
    SetDataDir(1);

  bitDelay();
  SetClockDir(1);

  bitDelay();

  return ack;
}
//======================================================================================================

void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
  // Write COMM1
  start();
  writeByte(TM1637_I2C_COMM1);
  stop();

  // Write COMM2 + first digit address
  start();
  writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

  // Write the data bytes
  for (uint8_t k = 0; k < length; k++)
    writeByte(segments[k]);

  stop();

  // Write COMM3 + brightness
  start();
  writeByte(TM1637_I2C_COMM3 + (m_brightness & 0x0f));
  stop();
}
//======================================================================================================

uint8_t encodeDigit(uint8_t digit)
{
  return digitToSegment[digit & 0x0f];
}
//======================================================================================================

void showNumberDec(int num, bool leading_zero, uint8_t length, uint8_t pos)
{
  uint8_t digits[4];
  const static int divisors[] = {1, 10, 100, 1000};
  bool leading = true;

  for (int8_t k = 0; k < 4; k++)
  {
    int divisor = divisors[4 - 1 - k];
    int d = num / divisor;

    if (d == 0)
    {
      if (leading_zero || !leading || (k == 3))
        digits[k] = encodeDigit(d);
      else
        digits[k] = 0;
    }
    else
    {
      digits[k] = encodeDigit(d);
      num -= d * divisor;
      leading = false;
    }
  }

  setSegments(digits + (4 - length), length, pos);
}
//======================================================================================================

#endif // __TM1637DISPLAY__
