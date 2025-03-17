#pragma once

#include "Serializer.hpp"
#include <functional>
#include <stdio.h>
#include <stdarg.h>

// Publisherクラス。Communicationクラスとデータを受け取り、シリアライズして送信する
template <typename Communication, typename... Args>
class Publisher {
    public:
        Publisher(Communication& communication, uint8_t frame_id)
         : communication(communication)
        {
            if (frame_id == 0) {
                return;
            }
            this->frame_id = frame_id; // frame_idを設定
        }

        void publish(const Args... args) {
            uint8_t buffer[Communication::BUFFER_SIZE];
            size_t size = 0;
            serialize(buffer, &size, args...); // シリアライズする。sizeにはシリアライズしたデータのサイズが入る
            communication.send(frame_id, buffer, size); // 送信(Communicationクラスに依存)
        }

    private:
        Communication& communication;
        uint8_t frame_id;
};

// Publisherクラス。char*の場合。printfが使える
template <typename Communication>
class Publisher<Communication, char*> {
    public:
        Publisher(Communication& communication, uint8_t frame_id)
         : communication(communication)
        {
            if (frame_id == 0) {
                return;
            }
            this->frame_id = frame_id; // frame_idを設定
        }

        void publish(char* data) {
            uint8_t buffer[Communication::BUFFER_SIZE];
            size_t size = 0;
            serialize(buffer, &size, data); // シリアライズする
            communication.send(frame_id, buffer, size); // 送信(Communicationクラスに依存)
        }

        // printfを使うための関数 (HALでは使えない)
        int printf(const char* format, ...) {
            va_list args;
            va_start(args, format);
            char buffer[Communication::BUFFER_SIZE];
            int size = vsnprintf(buffer, Communication::BUFFER_SIZE, format, args);
            va_end(args);
            publish(buffer);
            return size;
        }

    private:
        Communication& communication;
        uint8_t frame_id;
};



