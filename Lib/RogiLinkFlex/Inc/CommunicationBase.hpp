#pragma once

#include "CommunicationBase.hpp"
#include "Publisher.hpp"
#include "Subscriber.hpp"

#include <map>
#include <vector>
#include <functional>
#include <stdio.h>
#include <stdint.h>

// すべての通信クラスの基底クラス。送信処理とコールバック登録を行う
class CommunicationBase {
public:
    CommunicationBase(uint8_t device_id=0);

    void set_device_id(uint8_t device_id);

    // 送信処理(継承先で実装する)
    virtual void send(uint8_t frame_id, uint8_t* buffer, size_t size);
    
    // コールバック登録(Subscriberから呼ばれる)
    void add_callback(uint8_t frame_id, std::function<void(uint8_t*, size_t)> callback);

    // 受信時の処理。登録されたコールバックを呼ぶ
    void on_receive(uint8_t frame_id, /*uint8_t* buffer,*/ size_t size);

    void add_timeout_callback(uint8_t frame_id, uint32_t timeout, std::function<void()> callback);

    static const size_t BUFFER_SIZE = 256;

protected:
    // コールバック関数のマップ
    std::map<uint8_t, std::vector<std::function<void(uint8_t*, size_t)>>> callback_map;
    std::map<uint8_t, std::vector<std::tuple<uint32_t, std::function<void()>>>> timeout_callback_map;
    std::map<uint8_t, uint32_t> last_receive_time_map;

    // 受信時の処理(システムメッセージ)
    void on_received_system_message(uint8_t* buffer, size_t size);

    // デバイスID
    uint8_t device_id;
};

class CobsEncodedCommunicationBase : public CommunicationBase {
public:
    // コンストラクタ
    CobsEncodedCommunicationBase(uint8_t device_id=0);

    // 送信処理
    void send(uint8_t frame_id, uint8_t* data, size_t size) override;

protected:
    // 送信処理(継承先で実装する)
    virtual void send_raw(uint8_t* data, uint8_t size);

    // 受信処理(継承先から呼ばれる)
    void on_receive_raw(uint8_t* data, uint8_t size);

    // COBSエンコード
    static void encode_cobs(uint8_t *dst, uint8_t *src, uint8_t len);

    // COBSデコード
    static void decode_cobs(uint8_t *dst, uint8_t *src, uint8_t len);

    // Headerを付与
    static void add_header(uint8_t *dst, uint8_t *src, uint8_t len, uint8_t id);

    // Headerを削除し、IDとチェックサムを検証
    static void remove_header(uint8_t *dst, uint8_t *src, uint8_t len, uint8_t *id, bool *error);

    // チェックサムを計算
    static uint8_t calc_checksum(uint8_t *src, uint8_t len);

    static const uint8_t MAX_FRAME_LENGTH = 255;
};

