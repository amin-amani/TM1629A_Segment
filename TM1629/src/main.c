/*
vcc     
gnd
GPIO13  Data
GPIO12  CLK
GPIO14  STROB

*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"
#include "driver/uart.h"
#include "TM1637Display.h"


#define ACTIVATE 0x8f
#define BUTTONS 0x42
#define WRITE_LOC 0x44
#define WRITE_INC 0x40
#define BRIGHTNESS 0x80

#define BLINK_GPIO 2
#define TM162_CLK 22
#define TM162_Data 23
#define TM162_STROBE 14
#define TEST_DELAY 2000000

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};
//======================================================================================================
void GPIOInit()
{
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(TM162_CLK);
    gpio_set_direction(TM162_CLK, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(TM162_Data);
    gpio_set_direction(TM162_Data, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(TM162_STROBE);
    gpio_set_direction(TM162_STROBE, GPIO_MODE_OUTPUT);
}
//======================================================================================================

void delayMicroseconds(uint32_t delay)
{
 esp_rom_delay_us(delay);
}
//======================================================================================================

void SetClockPinDir(uint8_t dir)
{
  if (dir == 0)
   
    gpio_set_direction(TM162_CLK, GPIO_MODE_INPUT);
  else
    gpio_set_direction(TM162_CLK, GPIO_MODE_OUTPUT);
}
//======================================================================================================

void SetDataPinDir(uint8_t dir)
{
  if (dir == 0)
    gpio_set_direction(TM162_Data, GPIO_MODE_INPUT);
  else
    gpio_set_direction(TM162_Data, GPIO_MODE_OUTPUT);
}
//======================================================================================================

void SetClockValue(uint8_t value)
{
  if (value == 0)
    gpio_set_level(TM162_CLK, 0);
  else
    gpio_set_level(TM162_CLK, 1);
}
//======================================================================================================

void SetDataValue(uint8_t value)
{
  if (value == 0)
    gpio_set_level(TM162_Data, 0);
  else
    gpio_set_level(TM162_Data, 1);
}
//======================================================================================================

int ReadDataPinValue()
{
  return gpio_get_level(TM162_Data);
}
//======================================================================================================

void loop()
{
  int k;
  uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
  setBrightness(0x0f);

  // All segments on
  setSegments(data,4,0);
  Delay(TEST_DELAY);

  // Selectively set different digits
  data[0] = 0b01001001;
  data[1] = encodeDigit(1);
  data[2] = encodeDigit(2);
  data[3] = encodeDigit(3);

  for (k = 3; k >= 0; k--)
  {
    setSegments(data, 1, k);
    Delay(TEST_DELAY);
  }

  setSegments(data + 2, 2, 2);
  Delay(TEST_DELAY);

  setSegments(data + 2, 2, 1);
  Delay(TEST_DELAY);

  setSegments(data + 1, 3, 1);
  Delay(TEST_DELAY);

  // Show decimal numbers with/without leading zeros
  bool lz = false;
  for (uint8_t z = 0; z < 2; z++)
  {
    for (k = 0; k < 10000; k += k * 4 + 7)
    {
      showNumberDec(k, lz,4,0);
      Delay(TEST_DELAY);
    }
    lz = true;
  }

  // Show decimal number whose length is smaller than 4
  for (k = 0; k < 4; k++)
    data[k] = 0;
  setSegments(data,4,0);

  showNumberDec(153, false, 3, 1);
  Delay(TEST_DELAY);
  showNumberDec(22, false, 2, 2);
  Delay(TEST_DELAY);
  showNumberDec(0, true, 1, 3);
  Delay(TEST_DELAY);
  showNumberDec(0, true, 1, 2);
  Delay(TEST_DELAY);
  showNumberDec(0, true, 1, 1);
  Delay(TEST_DELAY);
  showNumberDec(0, true, 1, 0);
  Delay(TEST_DELAY);

  // Brightness Test
  for (k = 0; k < 4; k++)
    data[k] = 0xff;
  for (k = 0; k < 16; k++)
  {
    setBrightness(k);
    setSegments(data,4,0);
    Delay(TEST_DELAY);
  }

  // Done!
  setSegments(SEG_DONE,4,0);

  while (1)
    ;
}
//======================================================================================================
void app_main() 
{

    GPIOInit();
  TM1637Display(SetClockPinDir, SetClockValue,
                SetDataPinDir, SetDataValue,
                ReadDataPinValue, delayMicroseconds);
                loop();
    while (1)
    {
  
     vTaskDelay(500 / portTICK_PERIOD_MS);
    printf("Tick\n");

    }
    
}