#include "WS2815B.hpp"
#include "math.h"

#define NUM_LEDS 20
#define PI 3.14

WS2815B::WS2815B(TIM_HandleTypeDef* htim, uint32_t tim_channel_x){
    HTIM = htim;
    TIM_CHANNEL_X = tim_channel_x;
}

void WS2815B::set_rgb(uint16_t PIXEL_NUM, uint8_t red, uint8_t green, uint8_t blue){
    if(PIXEL_NUM<LED_NUM){
    rgb_buf[PIXEL_NUM][0]=red;
    rgb_buf[PIXEL_NUM][1]=green;
    rgb_buf[PIXEL_NUM][2]=blue;
    }
}

void WS2815B::show(){
pwm_buf_status = 1;
}

void WS2815B::execute(){
    if(pwm_buf_status==1){
    for(int i=0; i<RST; i++){
    pwm_buf[i]=0;
    }

    for(int i=0; i<LED_NUM; i++){
    for(int j=0; j<3; j++){
    for(int k=7; k>=0; k--){
    int x=(rgb_buf[i][j]>>k)&1;
    if(x==1){pwm_buf[RST+(i*24)+(j*8)+(7-k)]=HIGH;}
    else{pwm_buf[RST+(i*24)+(j*8)+(7-k)]=LOW;}
    }
    }      
}

HAL_TIM_PWM_Start_DMA(HTIM, TIM_CHANNEL_X, pwm_buf, RST+(LED_NUM*24));
pwm_buf_status = 0;
}else{

}
}

void WS2815B::init(){
for(int i=0; i<RST; i++){
        pwm_buf[i]=0;
}

for(int i=RST; i<LED_NUM*24; i++){
pwm_buf[i]=LOW;
}

HAL_TIM_PWM_Start_DMA(HTIM, TIM_CHANNEL_X, pwm_buf, RST+(LED_NUM*24));
}

void WS2815B::clear(){
for(int i=0; i<LED_NUM; i++){
rgb_buf[i][0]=0;
rgb_buf[i][1]=0;
rgb_buf[i][2]=0;
}
pwm_buf_status = 1;
}

// HSV（色相・彩度・明度）からRGBに変換し、RGBの合計が180以下になるようにスケール
void WS2815B::hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r_temp, g_temp, b_temp;

    if (h >= 0 && h < 60) { r_temp = c, g_temp = x, b_temp = 0; }
    else if (h >= 60 && h < 120) { r_temp = x, g_temp = c, b_temp = 0; }
    else if (h >= 120 && h < 180) { r_temp = 0, g_temp = c, b_temp = x; }
    else if (h >= 180 && h < 240) { r_temp = 0, g_temp = x, b_temp = c; }
    else if (h >= 240 && h < 300) { r_temp = x, g_temp = 0, b_temp = c; }
    else { r_temp = c, g_temp = 0, b_temp = x; }

    *r = (int)((r_temp + m) * 255);
    *g = (int)((g_temp + m) * 255);
    *b = (int)((b_temp + m) * 255);

    // RGBの合計が180を超えないようにスケール
    int sum = *r + *g + *b;
    if (sum > 180) {
        float scale = 180.0 / sum;
        *r = (int)(*r * scale);
        *g = (int)(*g * scale);
        *b = (int)(*b * scale);
    }
}

// LEDを12段階の色変化で往復させる
void WS2815B::smooth_color_transition() {
    float hue_start[] = {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330}; // 12段階の色
    float hue_end[] = {30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 360}; // 次の色にスムーズに移行
    int delay_start = 10; // 最初は遅く
    int delay_end = 3;   // 最後は速く

    for (int k = 0; k < 12; k++) { // 12段階の色変化
        for (int i = 0; i <= NUM_LEDS; i++) {
            int index = (k % 2 == 0) ? i : (NUM_LEDS - i);  // 偶数段階: 0→20, 奇数段階: 20→0

            float ratio = (float)i / NUM_LEDS; // 0〜1の割合
            float hue = hue_start[k] * (1 - ratio) + hue_end[k] * ratio; // 色相をスムーズに遷移
            int r, g, b;

            this->hsv_to_rgb(hue, 1.0, 0.8, &r, &g, &b); // 彩度1.0, 明度0.8で変換
            this->set_rgb(index, r, g, b);

            // 遅延時間を滑らかに短くする
            int delay_time = (int)(delay_start * (1 - ratio) + delay_end * ratio);
            HAL_Delay(delay_time);
            this->clear();
        }
    }
}