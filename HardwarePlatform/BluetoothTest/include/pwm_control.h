/*
* @file         pwm_LED.h 
* @brief        ESP32 PWM使用
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       ARCW
* @par Copyright (c):  
*               ARCW，QQ：329129674
* @par History:          
*               Ver0.0.1:
                     ARCW, 2019/07/08, 初始化版本\n 
*/

#ifndef _PWM_CONTROL_H_
#define _PWM_CONTROL_H_


#ifdef __cplusplus
extern "C"{
#endif


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"


#ifdef __cplusplus
}
#endif

/*
 * About this example
 *
 * 1. Start with initializing LEDC module:
 *    a. Set the timer of PWM first, this determines the frequency
 *       and resolution of PWM.
 *    b. Then set the PWM channel you want to use,
 *       and bind with one of the timers.
 *
 * 2. You need first to install a default fade function,
 *    then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example uses GPIO18/19 as pwm output,
 *    and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group.
 *    
 *
 */
#define PWM_HS_TIMER          LEDC_TIMER_0
#define PWM_HS_MODE           LEDC_HIGH_SPEED_MODE
#define PWM_HS_CH0_GPIO       (18)
#define PWM_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define PWM_HS_CH1_GPIO       (19)
#define PWM_HS_CH1_CHANNEL    LEDC_CHANNEL_1

// #define LEDC_LS_TIMER          LEDC_TIMER_1
// #define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
// #define LEDC_LS_CH2_GPIO       (4)
// #define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
// #define LEDC_LS_CH3_GPIO       (5)
// #define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

#define PWM_CH_NUM       (2)
#define PWM_DUTY         (4000) 
#define PWM_FADE_TIME    (1000)   //淡入的最大时间（ms）


/*
===========================
函数声明
=========================== 
*/
void pwm_init();
void pwm_duty_fade_change(const ledc_channel_config_t *pwm_channel,uint target_duty,uint max_fade_time_ms);
void pwm_duty_change(const ledc_channel_config_t *pwm_channel,uint target_duty);

extern  ledc_channel_config_t  pwm_channel1, pwm_channel2;

#endif