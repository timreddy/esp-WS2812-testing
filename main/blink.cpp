#include <iostream>
#include <string>
#include <math.h>

#include "WS2812.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BLINK_GPIO GPIO_NUM_5
#define BLINK_GPIO_POWER GPIO_NUM_8

static const char* TAG = "WS2812 Blink";

void ledBlinkRG(void* pParam) {
  WS2812 *led = (WS2812*)pParam;
  led->on();

  while(1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    led->set_rgb(0xFF, 0, 0);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    led->set_rgb(0, 0xFF, 0);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    led->set_rgb(0, 0, 0xFF);
  }
}

extern "C" void app_main() {
    WS2812 led(BLINK_GPIO_POWER, BLINK_GPIO);
    led.on();
    ESP_LOGI(TAG, "LED on");

    ESP_LOGI(TAG, "Testing power on/off");
    led.set_rgb(0xFF, 0x00, 0x00);
    vTaskDelay(pdMS_TO_TICKS(250));
    led.off();
    vTaskDelay(pdMS_TO_TICKS(250));
    led.on();
    vTaskDelay(pdMS_TO_TICKS(250));
    led.off();
    vTaskDelay(pdMS_TO_TICKS(250));
    led.on();

    TaskHandle_t led_task;
    xTaskCreate(ledBlinkRG, "WS2812_LED", 2048, &led, 10, &led_task);

    while(1) {
      xTaskNotifyGive(led_task);
      vTaskDelay(pdMS_TO_TICKS(250));
    }

    //should never get here. But, important to keep oled screen in scope
    vTaskSuspend(NULL);
}