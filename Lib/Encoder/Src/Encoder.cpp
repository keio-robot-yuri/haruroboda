#include "Encoder.hpp" //クラス定義を取得
#include "main.h"
#include <cmath> //円周率M_PIを使用

//コンストラクタ
//Encoderクラスを初期化
//htim:操作するタイマーのハンドルを設定
//タイマーハンドルをメンバ変数に格納
//resolution:分解能（１回転あたりのカウント数）を設定
//分解能をメンバ変数に格納
//count:累積カウント値を初期化
//累積カウント値を０に初期化
Encoder::Encoder(TIM_HandleTypeDef *htim, int16_t resolution) {
    this->htim = htim;
    this->resolution = resolution;
    this->count = 0;
}

//タイマーの開始
//HAL_TIM_Encoder_Start:STM32 HAL APIをしよhしてエンコーダモードを有効化
//タイマー周期（period）が0xFFFFでない場合、Error_Handlerを呼び出すて処理を停止
void Encoder::start() {
    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
    if(htim->Init.Period != 0xFFFF){
        // エラーを起こす
        Error_Handler();
    }
}

//カウント値の取得
//現在タイマーのカウント値を取得
//差分diffを計算して累積カウント値に加算
//エンコーダのオーバーフローやアンダーフローも計算に含む仕組み
int32_t Encoder::getRawCount() {
    int16_t current_count = (int16_t)__HAL_TIM_GET_COUNTER(htim);
    int16_t diff = current_count - prev_count;
    prev_count = current_count;
    count += diff;
    return count;
}

//角度や回転数の取得
//累積カウント値/１回転あたりのカウント数（分解能）= 回転数を計算
float Encoder::getRotations() {
    return (float)getRawCount() / (float)resolution;
}
//回転数を角度（度単位）に変換
float Encoder::getDegrees() {
    return getRotations() * 360.0f;
}
//回転数を角度（ラジアン単位）に変換
float Encoder::getRadians() {
    return getRotations() * 2.0f * M_PI;
}

//カウント値の設定
//生のカウント値（パルス数）を指定して設定、指定タイマーの現在アカウント値を０にリセット
//このリセットにより、以降のカウント値の変化が新しい基準値から始まるようになり、カウントの基準点を任意の値に設定できるため、ゼロ転合わせやリセット処理に役立つ
void Encoder::setRawCount(int32_t count) {
    this->count = count;
    __HAL_TIM_SET_COUNTER(htim, 0); // bufferをリセット
}
//回転数を指定して設定
//（処理内容）
//1.rotations * resolution：累積回転数（rotations）をカウント値に変換する。１回転あたりのパルス数（resolution）をかけることで、累積カウント値を計算
//2.setRawCount：上記で計算されたカウント値をsetRawCountにわたし、内部のカウント値を更新
void Encoder::setRotations(float rotations) {
    setRawCount((int32_t)(rotations * resolution));
}
//角度（度単位）を指定して設定
//（処理内容）
//1.degrees / 360.0f：累積角度を回転数に変換
//2.setRotations：上記で計算された回転数をsetRotationsに渡し、累積回転数を更新
void Encoder::setDegrees(float degrees) {
    setRotations(degrees / 360.0f);
}
//角度（ラジアン単位）を指定して設定
//（処理内容）
//1.radians / (2.0f * M_PI)：累積角度を回転数に変換
//2.setRotations：上記で計算された回転数をsetRotationsに渡し、累積回転数を更新
void Encoder::setRadians(float radians) {
    setRotations(radians / (2.0f * M_PI));
}