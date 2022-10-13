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
#define TM162_CLK 12
#define TM162_Data 13
#define TM162_STROBE 14


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
void SetClockPinDir(uint8_t dir)
{
  if (dir == 0)
   
    gpio_set_direction(TM162_CLK, GPIO_MODE_INPUT);
  else
    gpio_set_direction(TM162_CLK, GPIO_MODE_OUTPUT);
}
void SetDataPinDir(uint8_t dir)
{
  if (dir == 0)
   // pinMode(DIO, INPUT);
    gpio_set_direction(TM162_Data, GPIO_MODE_INPUT);
  else
    //pinMode(DIO, OUTPUT);
    gpio_set_direction(TM162_Data, GPIO_MODE_OUTPUT);
}
void SetClockValue(uint8_t value)
{
  if (value == 0)
    // digitalWrite(CLK, LOW);
    gpio_set_level(TM162_CLK, 0);
  else
    // digitalWrite(CLK, HIGH);
    gpio_set_level(TM162_CLK, 1);
}
void SetDataValue(uint8_t value)
{
  if (value == 0)
    // digitalWrite(DIO, LOW);
    gpio_set_level(TM162_Data, 0);
  else
    // digitalWrite(DIO, HIGH);
    gpio_set_level(TM162_Data, 1);
}
int ReadDataPinValue()
{
  // return digitalRead(DIO);
  return gpio_get_level(TM162_Data);
}
//======================================================================================================
void app_main() 
{

    GPIOInit();
    // TM1629Init();
  TM1637Display(SetClockPinDir, SetClockValue,
                SetDataPinDir, SetDataValue,
                ReadDataPinValue, delayMicroseconds);
    while (1)
    {
  
     vTaskDelay(500 / portTICK_PERIOD_MS);
    printf("Tick\n");
    // gpio_set_level(TM162_Data, 0);
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    // gpio_set_level(TM162_Data, 1);
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
}