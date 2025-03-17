#pragma once

#include <stdint.h>
#include "main.h"
#include <stm32f4xx_hal_tim.h>
#include <stm32f446xx.h>

class WS2815B{
    private:
    #define RST 250
    #define LED_NUM 20
    #define HIGH 10
    #define LOW 5
    uint8_t TIM_CHANNEL_X;
    TIM_HandleTypeDef* HTIM;
    uint8_t rgb_buf[LED_NUM][3];
    uint32_t pwm_buf[RST+((LED_NUM+1)*24)];

    public:
        WS2815B(TIM_HandleTypeDef* htim, uint32_t tim_channel_x);
        uint8_t pwm_buf_status;
        void set_rgb(uint16_t PIXEL_NUM, uint8_t red, uint8_t green, uint8_t blue);
        void show();
        void init();
        void execute();
        void clear();
};