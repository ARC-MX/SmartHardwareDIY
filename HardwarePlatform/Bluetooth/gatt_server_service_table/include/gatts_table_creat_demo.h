/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "freertos/event_groups.h"
 #include "esp_system.h"
 #include "esp_log.h"
 #include "nvs_flash.h"
 #include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"

#include "esp_gatt_common_api.h"

#define GATTS_LED_TAG "GATTS_TABLE_DEMO"

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55
#define SAMPLE_DEVICE_NAME          "ESP_GATTS_DEMO"
#define SVC_INST_ID                 0

/* The max length of characteristic value. When the gatt client write or prepare write, 
*  the data length must be less than GATTS_LED_CHAR_VAL_LEN_MAX. 
*/
#define GATTS_LED_CHAR_VAL_LEN_MAX  500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

#define CONFIG_SET_RAW_ADV_DATA

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

/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    LEDS_IDX_NB,
};

#ifdef __cplusplus
}
#endif