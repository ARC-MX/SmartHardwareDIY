



/*
* @file         hx_uart.c 
* @brief        ESP32双串口使用
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       hx-zsj 
* @par Copyright (c):  
*               红旭无线开发团队，QQ群：671139854
* @par History:          
*               Ver0.0.1:
                     hx-zsj, 2018/08/06, 初始化版本\n 
*/

/* 
=============
头文件包含
=============
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"

#include "uart_control.h"
#include "bluetooth.h"
#include "nvs_value.h"
/*
 * 应用程序的函数入口
 * @param[in]   NULL
 * @retval      NULL              
 * @par         修改日志 
 *               Ver0.0.1:
                     hx-zsj, 2018/08/06, 初始化版本\n  
*/

void esp32_init(){

    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    //esp_restart();



}

#ifdef __cplusplus
extern "C"{
#endif

void app_main();

#ifdef __cplusplus
}
#endif


void app_main()
{    
    nvs_init();
    printf("........uart_init().........\n");
	//uart_init();

    printf("..........bluetooth_init().......\n");
    bluetooth_init();
    printf(".........xTaskCreate........\n");
    //A uart read/write example without event queue;
    //xTaskCreate((TaskFunction_t) uart_echo, "uart_echo", RX1_BUF_SIZE, (void*)NULL, 10, NULL);
	//xTaskCreate(uart_task, "uart_task", BUF_SIZE * 2, (void*)UART_NUM_0, 12, NULL);
    printf(".................\n");

    nvs_write("pwm", "300");
    string value = nvs_read("pwm");
    printf("...........%s\n",value.c_str());
    cout <<  value << endl;

    nvs_write(BLUETOOTH_DEVICE_NAME_STORAGE,TEST_DEVICE_NAME);


    // std::string str= "value";
    // esp_gatt_value_t attr_value_a,attr_value;
    // set_gatt_rsp(&attr_value_a,str);

    // memcpy(attr_value.value,attr_value_a.value,str.length());

    //     esp_gatt_rsp_t rsp;
    //     memset(&rsp, 0, sizeof(esp_gatt_rsp_t));    //清空请求内存
        
    //     memcpy(rsp.attr_value.value,attr_value_a.value,str.length());
    // cout << "rsp.attr_value.value........"<<rsp.attr_value.value<<endl;

}