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
void app_main() 
{


    while (1)
    {
    printf("Tick\n");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
}