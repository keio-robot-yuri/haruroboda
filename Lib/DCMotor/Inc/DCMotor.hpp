#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stm32f4xx_hal_tim.h>
#include <stm32f446xx.h>

class DCMotor {
    public:
        DCMotor(TIM_HandleTypeDef *htim, uint16_t channel, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, bool direction=1, float max_duty=1.0f);
        void setDuty(float duty);
        void start();

    private:
        void setDirection(bool direction);
        TIM_HandleTypeDef *htim;
        uint16_t channel;
        bool direction;
        float current_duty;
        float max_duty;
        uint16_t pwm_resolution;

        GPIO_TypeDef* GPIO_Port;
        uint16_t GPIO_Pin;
};

#ifdef __cplusplus
}
#endif