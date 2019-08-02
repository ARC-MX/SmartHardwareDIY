/*
* @file         bluetooth.h 
* @brief        ESP32蓝牙使用
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       ARCW
* @par Copyright (c):  
*               ARCW，QQ：329129674
* @par History:          
*               Ver0.0.1:
                     ARCW, 2019/07/15, 初始化版本\n 
*/

/* 
=============
头文件包含
=============
*/

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_



#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"
#include "nvs_value.h"

#define GATTS_TAG "GATTS_DEMO"
#define GATTS_LED "LED_CONTROL"

#define GATTS_SERVICE_UUID_LED   0x00FF
#define GATTS_CHAR_UUID_LED      0xFF01
#define GATTS_DESCR_UUID_LED     0x3333
#define GATTS_NUM_HANDLE_LED     4           //此服务请求的句柄数。
#define LED_SREVICE_NUM          2

#define GATTS_SERVICE_UUID_TEST_B   0x00EE
#define GATTS_CHAR_UUID_TEST_B      0xEE01
#define GATTS_DESCR_UUID_TEST_B     0x2222
#define GATTS_NUM_HANDLE_TEST_B     4

#define TEST_DEVICE_NAME            "M_X"
#define MANUFACTURER_DATA           "王孟轩"  //制造商数据

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40

#define PREPARE_BUF_MAX_SIZE 1024

#define PROFILE_NUM 2               //定义两个profile
#define LED_CONTROL 0
#define PROFILE_B_APP_ID 1



#ifdef __cplusplus
extern "C"{
#endif




//定义Profile实例 结构体
struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;        //GATT通用连接接口参数
    uint16_t gatts_if;              //GATT通用连接接口
    uint16_t app_id;                //profile注册ID 表示当前服务index
    uint16_t conn_id;               //当前连接ID gatt连接事件参数
    uint16_t service_handle;        //GATT服务属性句柄
    esp_gatt_srvc_id_t service_id;  //服务信息 UUID和是否主服务 (uuid and instance id) and primary flag
    uint16_t char_handle;           //当前服务下characteristic句柄
    esp_bt_uuid_t char_uuid;        //characteristic UUID
    esp_gatt_perm_t perm;           // 属性权限 
    esp_gatt_char_prop_t property;  //characteristic 属性
    uint16_t descr_handle;          //描述句柄
    esp_bt_uuid_t descr_uuid;       //描述符UUID
};

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;


#ifdef __cplusplus
}
#endif

void set_device_name(std::string device_name);
void bluetooth_init();

#endif