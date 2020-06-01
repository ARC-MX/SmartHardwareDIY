/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
*
* This file is for gatt server. It can send adv data, be connected by client.
* Run the gatt_client demo, the client demo will automatically connect to the gatt_server_service_table demo.
* Client demo will enable gatt_server_service_table's notify after connection. Then two devices will exchange
* data.
*
****************************************************************************/

#include "bluetooth.h"


static uint8_t adv_config_done = 0;

uint16_t led_control_handle_table[LEDS_IDX_NB];

static prepare_type_env_t prepare_write_env;

#ifdef CONFIG_SET_RAW_ADV_DATA

static uint8_t raw_adv_data[] = {
    /* flags */
    0x02, 0x01, 0x06,
    /* tx power*/
    0x02, 0x0a, 0xeb,
    /* service uuid */
    0x03, 0x03, 0xFF, 0x00,
    /* device name */
    0x0f, 0x09, 'E', 'S', 'P', '_', 'G', 'A', 'T', 'T', 'S', '_', 'D', 'E', 'M', 'O'};
static uint8_t raw_scan_rsp_data[] = {
    /* flags */
    0x02, 0x01, 0x06,
    /* tx power */
    0x02, 0x0a, 0xeb,
    /* service uuid */
    0x03, 0x03, 0xFF, 0x00};
#else
static uint8_t service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb,
    0x34,
    0x9b,
    0x5f,
    0x80,
    0x00,
    0x00,
    0x80,
    0x00,
    0x10,
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
};

static std::string manufacturer = MANUFACTURER_DATA;
const uint8_t manufacturer_len = manufacturer.length();

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data;
// scan response data
static esp_ble_adv_data_t scan_rsp_data;
#endif /* CONFIG_SET_RAW_ADV_DATA */

static esp_ble_adv_params_t adv_params;

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst led_control_profile_tab[PROFILE_NUM];

/* Service */
static const uint16_t GATTS_LED_SERVICE_UUID = 0x00FF;
static const uint16_t GATTS_SWITCH_CHAR_UUID = 0xFF01;
static const uint16_t GATTS_TIMER_CHAR_UUID = 0xFF02;
static const uint16_t GATTS_BRIGHTNESS_CHAR_UUID = 0xFF03;
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_notify = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t heart_measurement_ccc[2] = {0x00, 0x00};

static char_values char_values_A = {"你是", "ABC", "BCD", ""};
static char_values char_values_B = {"我是", "QWE", "EWQ", ""};
static char_values char_values_C = {"你是", "ZXC", "VXZ", ""};

void set_char_value_A(std::string value)
{
    char_values_A.char_value = value;
}
 
void set_char_value_B(std::string value)
{
    char_values_B.char_value = value;
}

void set_char_value_C(std::string value)
{
    char_values_C.char_value = value;
}

void set_char_notify_value_A(std::string value)
{
    char_values_A.char_notify_value = value;
}

void set_char_indicate_value_A(std::string value)
{
    char_values_A.char_indicate_value = value;
}

static void set_gatt_rsp(esp_gatt_rsp_t *rsp, string value)
{
    rsp->attr_value.len = value.length(); //数据长度
    stringstream stream(value);
    stream >> rsp->attr_value.value;
}

/* Full Database Description - Used to add attributes into the database */
static esp_gatts_attr_db_t gatt_db[LEDS_IDX_NB] = {
    // // Service Declaration 服务声明
    // [IDX_SVC_PRIMARY] =
    //     {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATTS_LED_SERVICE_UUID), (uint8_t *)&GATTS_LED_SERVICE_UUID}},

    // /* Characteristic Declaration */ //特征值A声明
    // [IDX_CHAR_A] =
    //     {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    // /* Characteristic Value */ //特征值
    // [IDX_CHAR_VAL_A] =
    //     {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_SWITCH_CHAR_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, GATTS_LED_CHAR_VAL_LEN_MAX, (uint16_t)char_values_A.char_value.length(), (uint8_t *)char_values_A.char_value.c_str()}},

    // /* Client Characteristic Configuration Descriptor */ //客户端特征值描述
    // [IDX_CHAR_CFG_A] =
    //     {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

    // /* Characteristic Declaration */ //特征值B声明
    // [IDX_CHAR_B] =
    //     {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // /* Characteristic Value */ //特征值
    // [IDX_CHAR_VAL_B] =
    //     {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_TIMER_CHAR_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, GATTS_LED_CHAR_VAL_LEN_MAX, (uint16_t)char_values_B.char_value.length(), (uint8_t *)char_values_B.char_value.c_str()}},

    // /* Characteristic Declaration */ //特征值C声明
    // [IDX_CHAR_C] =
    //     {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},

    // /* Characteristic Value */ //特征值
    // [IDX_CHAR_VAL_C] =
    //     {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_BRIGHTNESS_CHAR_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, GATTS_LED_CHAR_VAL_LEN_MAX, (uint16_t)char_values_C.char_value.length(), (uint8_t *)char_values_C.char_value.c_str()}},

    // Service Declaration 服务声明
    [IDX_SVC_PRIMARY] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATTS_LED_SERVICE_UUID), (uint8_t *)&GATTS_LED_SERVICE_UUID}},

    /* Characteristic Declaration */ //特征值  LED开关  声明
    [IDX_CHAR_A] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},
    /* Characteristic Value */ //特征值 LED开关
    [IDX_CHAR_VAL_A] =
        {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_SWITCH_CHAR_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, GATTS_LED_CHAR_VAL_LEN_MAX, (uint16_t)char_values_A.char_value.length(), (uint8_t *)char_values_A.char_value.c_str()}},
    // /* Client Characteristic Configuration Descriptor */ //客户端特征值  LED开关 描述 
    // [IDX_CHAR_CFG_A] =
    //     {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},


    /* Characteristic Declaration */ //特征值  LED时间  声明
    [IDX_CHAR_B] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},
    /* Characteristic Value */ //特征值  LED时间 
    [IDX_CHAR_VAL_B] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_TIMER_CHAR_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, GATTS_LED_CHAR_VAL_LEN_MAX, (uint16_t)char_values_B.char_value.length(), (uint8_t *)char_values_B.char_value.c_str()}},
    //   /* Client Characteristic Configuration Descriptor */ //客户端特征值  LED时间 描述 
    // [IDX_CHAR_CFG_B] =
    //     {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},
    
    /* Characteristic Declaration */ //特征值LED亮度声明
    [IDX_CHAR_C] =
        {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,(uint8_t *)& char_prop_notify}},
    /* Characteristic Value */ //特征值  LED亮度
    [IDX_CHAR_VAL_C] =
        {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_BRIGHTNESS_CHAR_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, GATTS_LED_CHAR_VAL_LEN_MAX, (uint16_t)char_values_C.char_value.length(), (uint8_t *)char_values_C.char_value.c_str()}},
//   /* Client Characteristic Configuration Descriptor */ //客户端特征值  LED亮度 描述 
//     [IDX_CHAR_CFG_C] =
//         {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

};

static void struct_init()
{

    //gatts_profile_inst led_control_profile_tab[PROFILE_NUM]; =
    {
        {
            led_control_profile_tab[PROFILE_APP_IDX].gatts_cb = gatts_profile_event_handler;
            led_control_profile_tab[PROFILE_APP_IDX].gatts_if = ESP_GATT_IF_NONE; /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
        }
    }

#ifdef CONFIG_SET_RAW_ADV_DATA
#else
    //esp_ble_adv_data_t adv_data =
    {
        adv_data.set_scan_rsp = false;                                                //将广播数据设置为扫描回复数据
        adv_data.include_name = true;                                                 //是否广播设备名称
        adv_data.include_txpower = true;                                              //是否广播发射功率
        adv_data.min_interval = 0x0006;                                               //slave connection min interval; Time = min_interval * 1.25 msec
        adv_data.max_interval = 0x0010;                                               //slave connection max interval; Time = max_interval * 1.25 msec
        adv_data.appearance = 0x00;                                                   //设备外观
        adv_data.manufacturer_len = manufacturer_len;                                 //制造商数据长度
        adv_data.p_manufacturer_data = (uint8_t *)manufacturer.c_str();               //制造商数据
        adv_data.service_data_len = 0;                                                //服务数据长度
        adv_data.p_service_data = NULL;                                               //服务数据指针
        adv_data.service_uuid_len = 16;                                               //服务UUID长度
        adv_data.p_service_uuid = service_uuid;                                       //服务UUID数据指针
        adv_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT); //发现模式FLAG
    }

    //esp_ble_adv_data_t scan_rsp_data =
    {
        scan_rsp_data.set_scan_rsp = true;
        scan_rsp_data.include_name = true;
        scan_rsp_data.include_txpower = true;
        scan_rsp_data.min_interval = 0x0006;
        scan_rsp_data.max_interval = 0x0010;
        scan_rsp_data.appearance = 0x00;
        scan_rsp_data.manufacturer_len = manufacturer_len;                   //TEST_MANUFACTURER_DATA_LEN;
        scan_rsp_data.p_manufacturer_data = (uint8_t *)manufacturer.c_str(); //&test_manufacturer[0];
        scan_rsp_data.service_data_len = 0;
        scan_rsp_data.p_service_data = NULL;
        scan_rsp_data.service_uuid_len = 16;
        scan_rsp_data.p_service_uuid = service_uuid;
        scan_rsp_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    }
#endif
    //esp_ble_adv_params_t adv_params =
    {
        adv_params.adv_int_min = 0x20;
        adv_params.adv_int_max = 0x40;
        adv_params.adv_type = ADV_TYPE_IND;
        adv_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
        // adv_params.peer_addr            =
        // adv_params.peer_addr_type       =
        adv_params.channel_map = ADV_CHNL_ALL;
        adv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
#ifdef CONFIG_SET_RAW_ADV_DATA
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
#else
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
#endif
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        /* advertising start complete event to indicate advertising start successfully or failed */
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(GATTS_LED_TAG, "advertising start failed");
        }
        else
        {
            ESP_LOGI(GATTS_LED_TAG, "advertising start successfully");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(GATTS_LED_TAG, "Advertising stop failed");
        }
        else
        {
            ESP_LOGI(GATTS_LED_TAG, "Stop adv successfully\n");
        }
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(GATTS_LED_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                 param->update_conn_params.status,
                 param->update_conn_params.min_int,
                 param->update_conn_params.max_int,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}

void led_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(GATTS_LED_TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env->prepare_buf == NULL)
    {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        if (prepare_write_env->prepare_buf == NULL)
        {
            ESP_LOGE(GATTS_LED_TAG, "%s, Gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    }
    else
    {
        if (param->write.offset > PREPARE_BUF_MAX_SIZE)
        {
            status = ESP_GATT_INVALID_OFFSET;
        }
        else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE)
        {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp)
    {
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL)
        {
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK)
            {
                ESP_LOGE(GATTS_LED_TAG, "Send response error");
            }
            free(gatt_rsp);
        }
        else
        {
            ESP_LOGE(GATTS_LED_TAG, "%s, malloc failed", __func__);
        }
    }
    if (status != ESP_GATT_OK)
    {
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;
}

void led_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC && prepare_write_env->prepare_buf)
    {
        esp_log_buffer_hex(GATTS_LED_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }
    else
    {
        ESP_LOGI(GATTS_LED_TAG, "ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf)
    {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

static void handler_judge(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param, char_values data)
{


    cout<< "param->write.len = "<<param->write.len <<",param->write.handle= "<<param->write.handle<<", data.char_notify_value =  "<<data.char_notify_value.c_str()<<"  param->write.value = "<<param->write.value<<endl ;
    // if (led_control_handle_table[IDX_CHAR_CFG_A] == param->write.handle && param->write.len == 2)
    {
        uint16_t descr_value = param->write.value[1] << 8 | param->write.value[0];
        if (descr_value == 0x0001)
        {
            ESP_LOGI(GATTS_LED_TAG, "notify enable");

            //the size of notify_data[] need less than MTU size
            esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, param->write.handle,
                                        data.char_notify_value.length(), (uint8_t *)data.char_notify_value.c_str(), false);
        }
        else if (descr_value == 0x0002)
        {
            ESP_LOGI(GATTS_LED_TAG, "indicate enable");
            //the size of indicate_data[] need less than MTU size
            esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, param->write.handle,
                                        data.char_indicate_value.length(), (uint8_t *)data.char_indicate_value.c_str(), true);
        }
        else if (descr_value == 0x0000)
        {
            ESP_LOGI(GATTS_LED_TAG, "notify/indicate disable ");
        }
        else
        {
            ESP_LOGE(GATTS_LED_TAG, "unknown descr value");
            esp_log_buffer_hex(GATTS_LED_TAG, param->write.value, param->write.len);
        }
    }
    /* send response when param->write.need_rsp is true*/
    if (param->write.need_rsp)
    {
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));     //清空请求内存
        rsp.attr_value.handle = param->write.handle; //attribute handle属性句柄
        set_gatt_rsp(&rsp, data.char_write_rsp_value);
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, &rsp);
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    {
        esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(BLUETOOTH_DEVICE_NAME);
        if (set_dev_name_ret)
        {
            ESP_LOGE(GATTS_LED_TAG, "set device name failed, error code = %x", set_dev_name_ret);
        }
#ifdef CONFIG_SET_RAW_ADV_DATA
        esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
        if (raw_adv_ret)
        {
            ESP_LOGE(GATTS_LED_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
        }
        adv_config_done |= ADV_CONFIG_FLAG;
        esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
        if (raw_scan_ret)
        {
            ESP_LOGE(GATTS_LED_TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
        }
        adv_config_done |= SCAN_RSP_CONFIG_FLAG;
#else
        //config adv data
        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret)
        {
            ESP_LOGE(GATTS_LED_TAG, "config adv data failed, error code = %x", ret);
        }
        adv_config_done |= ADV_CONFIG_FLAG;
        //config scan response data
        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret)
        {
            ESP_LOGE(GATTS_LED_TAG, "config scan response data failed, error code = %x", ret);
        }
        adv_config_done |= SCAN_RSP_CONFIG_FLAG;
#endif
        esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, LEDS_IDX_NB, SVC_INST_ID);
        if (create_attr_ret)
        {
            ESP_LOGE(GATTS_LED_TAG, "create attr table failed, error code = %x", create_attr_ret);
        }
        break;
    }

    //GATT读取事件
    case ESP_GATTS_READ_EVT:
    {
        // cout << "handle of led_control_handle_table[IDX_CHAR_CFG_A] is " << led_control_handle_table[IDX_CHAR_CFG_A] << endl;
        cout << "handle of led_control_handle_table[IDX_CHAR_VAL_A] is " << led_control_handle_table[IDX_CHAR_VAL_A] << endl;
        cout << "handle of led_control_handle_table[IDX_CHAR_VAL_B] is " << led_control_handle_table[IDX_CHAR_VAL_B] << endl;
        cout << "handle of led_control_handle_table[IDX_CHAR_VAL_C] is " << led_control_handle_table[IDX_CHAR_VAL_C] << endl;

        ESP_LOGI(GATTS_LED_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));    //清空请求内存
        rsp.attr_value.handle = param->read.handle; //attribute handle属性句柄

        //选定回复特征
        if (led_control_handle_table[IDX_CHAR_VAL_A] == param->read.handle)
        {
            set_gatt_rsp(&rsp, char_values_A.char_value);
            //设定GATT回复内容
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp); //发送回复
            break;
        }
        else if (led_control_handle_table[IDX_CHAR_VAL_B] == param->read.handle)
        {
            set_gatt_rsp(&rsp, char_values_B.char_value);
            //设定GATT回复内容
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp); //发送回复
            break;
        }
        else if (led_control_handle_table[IDX_CHAR_VAL_C] == param->read.handle)
        {
            set_gatt_rsp(&rsp, char_values_C.char_value);
            //设定GATT回复内容
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp); //发送回复
            break;
        }
    }
        //写入事件

    case ESP_GATTS_WRITE_EVT:
    {
        if (!param->write.is_prep)
        {
            // the data length of gattc write  must be less than GATTS_LED_CHAR_VAL_LEN_MAX.
            ESP_LOGI(GATTS_LED_TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", param->write.handle, param->write.len);
            esp_log_buffer_hex(GATTS_LED_TAG, param->write.value, param->write.len);
            cout << "PROFILE_LED param->write.value = " << param->write.value << "     param->write.len = " << (int)param->write.len << endl;
            cout << "PROFILE_LED param->write.handle =" << param->write.handle << endl;
            //选定回复特征
            if (/*led_control_handle_table[IDX_CHAR_CFG_A] == param->write.handle ||*/ led_control_handle_table[IDX_CHAR_VAL_A] == param->write.handle)
            {

               // if (led_control_handle_table[IDX_CHAR_VAL_A] == param->write.handle && param->write.len == 2)
                handler_judge(gatts_if, param, char_values_A);
                
            }
            if (led_control_handle_table[IDX_CHAR_VAL_B] == param->write.handle)
            {
                handler_judge(gatts_if, param, char_values_B);
                
            }
            if (led_control_handle_table[IDX_CHAR_VAL_C] == param->write.handle)
            {
                handler_judge(gatts_if, param, char_values_C);
                
            }
        }
        else
        {
            /* handle prepare write */
            led_prepare_write_event_env(gatts_if, &prepare_write_env, param);
        }
        break;
    }

    case ESP_GATTS_EXEC_WRITE_EVT:
    { // the length of gattc prapare write data must be less than GATTS_LED_CHAR_VAL_LEN_MAX.
        ESP_LOGI(GATTS_LED_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
        //LED执行写入操作
        led_exec_write_event_env(&prepare_write_env, param);
        break;
    }

    case ESP_GATTS_MTU_EVT:
    {
        ESP_LOGI(GATTS_LED_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    }

    case ESP_GATTS_CONF_EVT:
    {
        ESP_LOGI(GATTS_LED_TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", param->conf.status, param->conf.handle);
        break;
    }

    case ESP_GATTS_START_EVT:
    {
        ESP_LOGI(GATTS_LED_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
        break;
    }

    case ESP_GATTS_CONNECT_EVT:
    {
        ESP_LOGI(GATTS_LED_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
        esp_log_buffer_hex(GATTS_LED_TAG, param->connect.remote_bda, 6);
        esp_ble_conn_update_params_t conn_params = {};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20; // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10; // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;  // timeout = 400*10ms = 4000ms
        //start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }

    case ESP_GATTS_DISCONNECT_EVT:
    {
        ESP_LOGI(GATTS_LED_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    }

    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
    {
        if (param->add_attr_tab.status != ESP_GATT_OK)
        {
            ESP_LOGE(GATTS_LED_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        }
        else if (param->add_attr_tab.num_handle != LEDS_IDX_NB)
        {
            ESP_LOGE(GATTS_LED_TAG, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to LEDS_IDX_NB(%d)",
                     param->add_attr_tab.num_handle, LEDS_IDX_NB);
        }
        else
        {
            ESP_LOGI(GATTS_LED_TAG, "create attribute table successfully, the number handle = %d\n", param->add_attr_tab.num_handle);
            memcpy(led_control_handle_table, param->add_attr_tab.handles, sizeof(led_control_handle_table));
            esp_ble_gatts_start_service(led_control_handle_table[IDX_SVC_PRIMARY]);
        }
        break;
    }
    case ESP_GATTS_STOP_EVT:
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    case ESP_GATTS_UNREG_EVT:
    case ESP_GATTS_DELETE_EVT:
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{

    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT)
    {
        if (param->reg.status == ESP_GATT_OK)
        {
            led_control_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        }
        else
        {
            ESP_LOGE(GATTS_LED_TAG, "reg app failed, app_id %04x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }
    do
    {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++)
        {
            /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == led_control_profile_tab[idx].gatts_if)
            {
                if (led_control_profile_tab[idx].gatts_cb)
                {
                    led_control_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void set_device_name(std::string device_name)
{
    //设定GAP协议中蓝牙设备名称
    esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(device_name.c_str());
    if (set_dev_name_ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "set device name failed, error code = %x", set_dev_name_ret);
    }
    ESP_LOGE(GATTS_LED_TAG, "set device name = %s secceed", device_name.c_str());
    nvs_write(BLUETOOTH_DEVICE_NAME_STORAGE, device_name);
}

void bluetooth_init()
{
    //初始化所有结构体变量
    struct_init();
    esp_err_t ret;
    //蓝牙控制器按照经典蓝牙模式释放内存
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    //获取蓝牙默认初始化控制参数
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    //蓝牙设备默认初始化
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    //蓝牙设备使能BLE模式
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    //bluedroid初始化
    ret = esp_bluedroid_init();
    if (ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    //bluedroid使能
    ret = esp_bluedroid_enable();
    if (ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    //设定器件名称
    set_device_name(nvs_read(BLUETOOTH_DEVICE_NAME_STORAGE));

    //注册GATT事件
    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "gatts register error, error code = %x", ret);
        return;
    }

    //注册GAP事件
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "gap register error, error code = %x", ret);
        return;
    }

    //注册GATT应用（profileLED）
    ret = esp_ble_gatts_app_register(ESP_APP_ID);
    if (ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    //设定GATT  MTU
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret)
    {
        ESP_LOGE(GATTS_LED_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    return;
}