#pragma once

#include "stm32f4xx_hal.h"
#include <stm32f446xx.h>

namespace RoboMasterValues {
    enum ID {
        FirstFour = 0x200,
        SecondFour = 0x144,
        ReceiveZeroPoint = 0x201,
    };

    enum class ControlMode {
        Current = 0,
        Velocity = 1,
        Position = 2
    };

    const int RESOLUTION = 8192;
};

typedef struct {
    float pos_radian_absolute = 0;
    float pos_radian_estimated = 0;
    float vel_rps = 0;
    uint16_t current = 0;
    uint8_t temperature = 0;
} receive_data_t;

typedef struct {
    RoboMasterValues::ControlMode mode = RoboMasterValues::ControlMode::Velocity;
    float pos_radian = 0;
    float vel_rps = 0;
    uint16_t current = 0;
} target_data_t;

typedef struct {
    float speed_kp = 35.0f;
    float speed_ki = 0.15f;
    float pos_kp = 1.0f;
    float pos_ki = 0.f;
    float pos_kd = 0.0f;
    float max_speed = 200.0f;
} pid_gain_t;

class RoboMasterController {
    public:
        RoboMasterController(CAN_HandleTypeDef* hcan_ptr, bool disable_first_four = false, bool disable_second_four = true);
        void start();
        void send(uint32_t id, uint8_t* data, uint32_t size);
        void send_force(uint32_t id, uint8_t* data, uint32_t size);

        void timer_callback();

        void setCurrentFirstFour(int16_t current0, int16_t current1, int16_t current2, int16_t current3);
        void setCurrentSecondFour(int16_t current4, int16_t current5, int16_t current6, int16_t current7);

        void onReceive(uint8_t id, uint8_t* data, uint8_t size);

        void setPIDGain(uint8_t id, float speed_kp, float pos_kp);
        void setControlMode(uint8_t id, RoboMasterValues::ControlMode mode);
        void setTargetSpeed(uint8_t id, float velocity);
        void setTargetPosition(uint8_t, float position);
        void setTargetCurrent(uint8_t, int16_t current);

        bool timer_flag = false;

        receive_data_t receive_data[8];
        pid_gain_t pid_gain[8];

    private:
        bool disable_first_four, disable_second_four;
        CAN_HandleTypeDef* hcan_ptr;
        target_data_t target[8];
        float speed_error_integral[8] = {0};
        float pos_error_integral[8] = {0};
        float pos_error_prev[8] = {0};
        int pos_counts[8] = {0};
};