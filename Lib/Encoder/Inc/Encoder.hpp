#pragma once // 同じヘッダーファイルが複数回インクルードされても、コンパイラが１回だけ処理するようになる

#include "stm32f4xx_hal.h" //STM32F4シリーズのHALドライバ全般を提供
#include <stdint.h>
#include <stm32f4xx_hal_tim.h> //タイマー周辺機能を扱うためのHAL　API
#include <stm32f446xx.h> //STM32F446REマイクロコントローラ固有の定義

//エンコーダクラス（Encoder）のインスタンスを初期化するために使用
//使用するタイマーとエンコーダの分解能を指定して、エンコーダの設定を行う
class Encoder {
    public:
        //TIM_HandleTypeDef *htim:（型）STM32HALドライバが提供するタイマー操作用の構造体
        //                       :（役割）エンコーダとして使用するタイマーのハンドルポインタ
        //                       :（例）TIM3をエンコーダモードで動作させたい場合、その設定済みハンドルを渡す
        //int16_t resolution:（型）int16_t - 符号付き１６ビット整数
        //                  :（役割）エンコーダの分解能（１回転あたりのパルス数）を指定
        Encoder(TIM_HandleTypeDef *htim, int16_t resolution);
        void start(); //エンコーダタイマーの動作開始

        // カウント取得
        int32_t getRawCount(); //パルス数を返す
        float getRotations(); //回転数（１回転を１とした値）を返す
        float getDegrees(); //角度（度単位）を返す。
        float getRadians(); //角度（ラジアン単位）を返す。

        // リセット
        void setRawCount(int32_t count); //生のカウント値（パルス数）を指定して設定
        void setRotations(float rotations); //回転数を指定して設定
        void setDegrees(float degrees); //角度（度単位）を指定して設定
        void setRadians(float radians); //角度（ラジアン単位）を指定して設定

    private:
        TIM_HandleTypeDef *htim; //エンコーダのタイマーを操作するためのハンドラ
        uint16_t channel; //未使用（削除可能）
        int16_t resolution; //１回転あたりのカウント数（分解能）
        int32_t count; //累積カウント値を格納する変数
        int16_t prev_count; //前回のカウント値（差分計算に利用）
};