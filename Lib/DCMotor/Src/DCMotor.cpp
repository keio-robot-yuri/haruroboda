#include "DCMotor.hpp"

DCMotor::DCMotor(TIM_HandleTypeDef *htim, uint16_t channel, GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin, bool direction, float max_duty) {
    this->htim = htim;
    this->channel = channel;
    this->direction = direction;
    this->GPIO_Port = GPIO_Port;
    this->GPIO_Pin = GPIO_Pin;
    this->max_duty = max_duty < 1.0f ? max_duty : 1.0f;
}

void DCMotor::start() {
    HAL_TIM_PWM_Start(htim, channel);
    pwm_resolution = htim->Init.Period;
}

void DCMotor::setDuty(float duty) {
    current_duty = duty;
    uint32_t value;
    if (duty > max_duty) {
        duty = max_duty;
    }else if (duty < -max_duty) {
        duty = -max_duty;
    }
    
    if (duty > 0){
        value = (uint32_t)(htim->Init.Period * (1.0f-duty));
        setDirection(1);
    }else{
        value = (uint32_t)(htim->Init.Period * (1.0f+duty));
        setDirection(0);
    }
    __HAL_TIM_SET_COMPARE(htim, channel, value);
}

void DCMotor::setDirection(bool direction) {
    if (direction == this->direction) {
        HAL_GPIO_WritePin(GPIO_Port, GPIO_Pin, GPIO_PIN_SET);
    }else{
        HAL_GPIO_WritePin(GPIO_Port, GPIO_Pin, GPIO_PIN_RESET);
    }
}
