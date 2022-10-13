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
void SendDataMSB(uint8_t data)
{
int i;
  gpio_set_level(TM162_STROBE, 0);
   vTaskDelay(1 / portTICK_PERIOD_MS);
    for(i=0;i<8;i++)
    {
    vTaskDelay(1 / portTICK_PERIOD_MS); 
    gpio_set_level(TM162_Data, data&0x80);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(TM162_CLK, 0);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    gpio_set_level(TM162_CLK, 1);
    data<<=1;
    }
     vTaskDelay(1 / portTICK_PERIOD_MS);
  gpio_set_level(TM162_STROBE, 1);

}
void SendData(uint8_t data)
{
int i;
  gpio_set_level(TM162_STROBE, 0);
   vTaskDelay(1 / portTICK_PERIOD_MS);
    for(i=0;i<8;i++)
    {
    vTaskDelay(1 / portTICK_PERIOD_MS); 
    gpio_set_level(TM162_Data, data&0x1);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(TM162_CLK, 0);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    gpio_set_level(TM162_CLK, 1);
    data>>=1;
    }
     vTaskDelay(1 / portTICK_PERIOD_MS);
  gpio_set_level(TM162_STROBE, 1);

}


void TM1629Reset() 
{
  SendData(WRITE_INC); // set auto increment mode
  gpio_set_level(TM162_STROBE, 0);
  SendData(0xc0);   // set starting address to 0
  for (uint8_t i = 0; i < 16; i++)
  {
    SendData( 0x00);
  }
  gpio_set_level(TM162_STROBE, 1);
  SendData(WRITE_INC); // set auto increment mode
  SendData(ACTIVATE);
}
//======================================================================================================

void TM1629Init()
{
    gpio_set_level(TM162_STROBE, 1);
    gpio_set_level(TM162_CLK, 1);
    gpio_set_level(TM162_Data, 1);
    SendData(ACTIVATE);
    TM1629Reset();
}
void UpdateDisplay()
{
  

  SendData(WRITE_INC);
  gpio_set_level(TM162_STROBE, 0);
  SendData(0xc0);   // set starting address to 0
  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t tempData = 0x00;  //SEG1-8
    for (uint8_t j=0; j<8; j++){
      tempData |=( 0x0a/*by amin*/ & (0x80 >> i)) << i >> j;
    }
  }
  gpio_set_level(TM162_STROBE, 1);

}
//======================================================================================================
void app_main() 
{

    GPIOInit();
    TM1629Init();

    while (1)
    {
       UpdateDisplay();
     vTaskDelay(500 / portTICK_PERIOD_MS);
    printf("Tick\n");
    // gpio_set_level(TM162_Data, 0);
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    // gpio_set_level(TM162_Data, 1);
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
}