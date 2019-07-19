/*
* @file         uart.h 
* @brief        ESP32串口使用
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       ARCW
* @par Copyright (c):  
*               ARCW，QQ：329129674
* @par History:          
*               Ver0.0.1:
                     ARCW, 2019/07/06, 初始化版本\n 
*/

/* 
=============
头文件包含
=============
*/

#ifndef _UART_CONTROL_H_ 
#define _UART_CONTROL_H_ 

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"

#ifdef __cplusplus
}
#endif



//UART0
#define RX0_BUF_SIZE 		(1024)
#define TX0_BUF_SIZE 		(1024)

#define BUF_SIZE (1024)

//UART1
#define RX1_BUF_SIZE 		(1024)
#define TX1_BUF_SIZE 		(512)
#define TXD1_PIN 			(GPIO_NUM_4)
#define RXD1_PIN 			(GPIO_NUM_5)

// //UART2
// #define RX2_BUF_SIZE 		(1024)
// #define TX2_BUF_SIZE 		(512)
// #define TXD2_PIN 			(GPIO_NUM_12)
// #define RXD2_PIN 			(GPIO_NUM_13)


/*

===========================
函数声明
=========================== 
*/

void uart_task(void *pvParameters);

void uart_echo();

// void uart1_rx_task();
// void uart2_rx_task();
void uart_init();
void uart1_init();
void uart0_init();
// void spp_uart_init(void);
// void uart2_init();
//void IRAM_ATTR uart1_irq_handler(void *arg);


#endif

