#pragma once

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <type_traits>

namespace BNO055_Register {
    enum {
        OPR_MODE = 0x3D,
        EUL_DATA_X_LSB = 0x1A,
        EUL_DATA_X_MSB = 0x1B,
        EUL_DATA_Y_LSB = 0x1C,
        EUL_DATA_Y_MSB = 0x1D,
        EUL_DATA_Z_LSB = 0x1E,
        EUL_DATA_Z_MSB = 0x1F,
    };

    enum OPR_MODE_VALUE {
        CONFIG_MODE = 0b0000,
        ACCONLY_MODE = 0b0001,
        MAGONLY_MODE = 0b0010,
        GYRONLY_MODE = 0b0011,
        ACCMAG_MODE = 0b0100,
        ACCGYRO_MODE = 0b0101,
        MAGGYRO_MODE = 0b0110,
        AMG_MODE = 0b0111,
        IMU_MODE = 0b1000,
        M4G_MODE = 0b1010,
        NDOF_FMC_OFF_MODE = 0b1011,
        NDOF_MODE = 0b1100,
    };

    enum FIRST_BYTE {
        START = 0xAA,
        SUCCESS = 0xBB,
        FAIL = 0xEE,
    };

    enum SECOND_BYTE {
        READ = 0x01,
        WRITE = 0x00,
    };
};

enum class EUL_AXIS {
    EUL_X = 0,
    EUL_Y = 1,
    EUL_Z = 2,
};

enum class BNO055_UART_STATE {
    IDLE,
    WAIT_FOR_HEADER,
    WAIT_FOR_DATA,
};

class BNO055_UART {
public:
    BNO055_UART(UART_HandleTypeDef* huart, EUL_AXIS eul=EUL_AXIS::EUL_X);
    ~BNO055_UART();

    // センサーの初期化、通信開始
    void start();

    // 1回目の受信を完了したかどうか
    bool is_ready();

    // オイラー角の送信要求し、受信完了後に角度を取得する
    void request_euler();

    // 角度を取得する[deg]
    float get_degrees();

    // 角度を取得する[rad]
    float get_radians();

    // BNO055レジスタにデータを書き込む[複数バイト]
    void write_register(uint8_t reg, uint8_t length, uint8_t* data);

    // BNO055レジスタにデータを書き込む[1バイト]
    void write_register(uint8_t reg, uint8_t data);

    // BNO055レジスタからデータを読み込む
    void read_register_it(uint8_t reg, uint8_t length, uint8_t* data);

    // UART受信割り込み
    void interrupt();

    void reset();

    bool request_flag = false;

private:
    // 分解能
    static const int EULER_RESOLUTION = 5760;

    // オイラー角の生データ
    int16_t raw_euler_value = 0;

    // 前回のオイラー角の生データ
    int16_t last_raw_euler_value = 0;

    //零点
    int32_t zero_point = 0;

    // UARTハンドラ
    UART_HandleTypeDef* huart_ptr;

    // オイラー角の軸
    EUL_AXIS eul;

    // 受信バッファ
    uint8_t receive_buffer[256];

    // 1回目の受信完了フラグ
    bool ready = false;

    // 次回受信データ格納先のポインタ
    uint8_t* receive_ptr = nullptr;

    // 受信状態
    BNO055_UART_STATE state = BNO055_UART_STATE::IDLE;

    // 次回受信するデータの長さ
    uint8_t receiving_length = 0;

    void update();
};

