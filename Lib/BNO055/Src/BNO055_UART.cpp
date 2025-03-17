#include "BNO055_UART.hpp"
#include <math.h>
#include <string.h>

BNO055_UART::BNO055_UART(UART_HandleTypeDef* huart, EUL_AXIS eul) {
    this->huart_ptr = huart;
    this->eul = eul;
    this->raw_euler_value = 0;
    this->last_raw_euler_value = 0;
    this->zero_point = 0;
    this->ready = false;
    this->state = BNO055_UART_STATE::IDLE;
    this->receiving_length = 0;
    this->receive_ptr = nullptr;
}

BNO055_UART::~BNO055_UART() {

}

void BNO055_UART::start() {
    write_register(BNO055_Register::OPR_MODE, BNO055_Register::OPR_MODE_VALUE::NDOF_MODE); // 9軸モード
    HAL_Delay(50);
}

void BNO055_UART::request_euler() {
    uint8_t reg = 0;
    switch (eul) {
        case EUL_AXIS::EUL_X:
            reg = BNO055_Register::EUL_DATA_X_LSB;
            break;
        case EUL_AXIS::EUL_Y:
            reg = BNO055_Register::EUL_DATA_Y_LSB;
            break;
        case EUL_AXIS::EUL_Z:
            reg = BNO055_Register::EUL_DATA_Z_LSB;
            break;
    }
    read_register_it(reg, 2, (uint8_t*)&raw_euler_value);
}

bool BNO055_UART::is_ready() {
    return ready;
}

float BNO055_UART::get_degrees() {
    return - (float)(raw_euler_value - zero_point) / EULER_RESOLUTION * 360;
}

float BNO055_UART::get_radians() {
    return - (float)(raw_euler_value - zero_point) / EULER_RESOLUTION * 2 * M_PI;
}


void BNO055_UART::write_register(uint8_t reg, uint8_t length, uint8_t* data) {
    uint8_t buffer[256];
    buffer[0] = BNO055_Register::FIRST_BYTE::START;
    buffer[1] = BNO055_Register::SECOND_BYTE::WRITE;
    buffer[2] = reg;
    memcpy(buffer + 3, data, length);
    HAL_UART_Transmit(huart_ptr, buffer, length + 3, 100);
}

void BNO055_UART::write_register(uint8_t reg, uint8_t data) {
    write_register(reg, 1, &data);
}

void BNO055_UART::read_register_it(uint8_t reg, uint8_t length, uint8_t* data) {
    uint8_t buffer[256];
    buffer[0] = BNO055_Register::FIRST_BYTE::START;
    buffer[1] = BNO055_Register::SECOND_BYTE::READ;
    buffer[2] = reg;
    buffer[3] = length;
    HAL_UART_Transmit(huart_ptr, buffer, 4, 20);
    receive_ptr = data;
    receiving_length = length;
    HAL_UART_Receive_IT(huart_ptr, receive_buffer, 2); // 2バイト受信
    state = BNO055_UART_STATE::WAIT_FOR_HEADER;
}

// 受信割り込み
void BNO055_UART::interrupt() {
    if (state == BNO055_UART_STATE::WAIT_FOR_HEADER) { // ヘッダ受信
        if (receive_buffer[0] == BNO055_Register::FIRST_BYTE::SUCCESS && receive_buffer[1] == receiving_length) {
            state = BNO055_UART_STATE::WAIT_FOR_DATA;
            HAL_UART_Receive_IT(huart_ptr, receive_buffer, receiving_length);
        } else {
            state = BNO055_UART_STATE::IDLE;
        }
    } else if (state == BNO055_UART_STATE::WAIT_FOR_DATA) { // データ受信
        if (receive_ptr != nullptr){
            memcpy(receive_ptr, receive_buffer, receiving_length);
            update();
        }
        state = BNO055_UART_STATE::IDLE;
    }
}

void BNO055_UART::update() {
    if(last_raw_euler_value > 3 * EULER_RESOLUTION / 4 && raw_euler_value < EULER_RESOLUTION / 4){
        zero_point -= EULER_RESOLUTION;
    }else if(last_raw_euler_value < EULER_RESOLUTION / 4 && raw_euler_value > 3 * EULER_RESOLUTION / 4){
        zero_point += EULER_RESOLUTION;
    }
    last_raw_euler_value = raw_euler_value;

    if (!ready){
        reset();
        ready = true;
    }
}

void BNO055_UART::reset() {
    zero_point = raw_euler_value;
}