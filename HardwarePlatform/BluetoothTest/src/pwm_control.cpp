/*
* @file         pwm_LED.c 
* @brief        ESP32 PWM使用
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       ARCW
* @par Copyright (c):  
*               ARCW，QQ：329129674
* @par History:          
*               Ver0.0.1:
                     ARCW, 2019/07/08, 初始化版本\n 
*/

/* 
=============
头文件包含
=============
*/
#include "pwm_control.h"
/* 
===========================
全局变量定义
=========================== 
*/


/*
    * Prepare individual configuration
    * for each channel of LED Controller
    * by selecting:
    * - controller's channel number
    * - output duty cycle, set initially to 0
    * - GPIO number where LED is connected to
    * - speed mode, either high or low
    * - timer servicing selected channel
    *   Note: if different channels use one timer,
    *         then frequency and bit_num of these channels
    *         will be the same
    */


ledc_channel_config_t  pwm_channel1, pwm_channel2;

void pwm_init(){
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t pwm_timer0_config;

    pwm_timer0_config.speed_mode = PWM_HS_MODE;             // timer mode   高速模式
    pwm_timer0_config.duty_resolution = LEDC_TIMER_10_BIT;  // resolution of PWM duty   pwm精度设置成10位
    pwm_timer0_config.freq_hz = 20000;                      // frequency of PWM signal  pwm频率20K
    pwm_timer0_config.timer_num = PWM_HS_TIMER;             // timer index  定时器源

    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&pwm_timer0_config);

    // Set PWM Controller with previously prepared configuration


    pwm_channel1.channel    = PWM_HS_CH0_CHANNEL;
    pwm_channel1.duty       = 0;
    pwm_channel1.gpio_num   = PWM_HS_CH0_GPIO;
    pwm_channel1.speed_mode = PWM_HS_MODE;
    pwm_channel1.hpoint     = 0;
    pwm_channel1.timer_sel  = PWM_HS_TIMER;

    pwm_channel2.channel    = PWM_HS_CH1_CHANNEL;
    pwm_channel2.duty       = 0;
    pwm_channel2.gpio_num   = PWM_HS_CH1_GPIO;
    pwm_channel2.speed_mode = PWM_HS_MODE;
    pwm_channel2.hpoint     = 0;
    pwm_channel2.timer_sel  = PWM_HS_TIMER;


    ledc_channel_config(&pwm_channel1);
    ledc_channel_config(&pwm_channel2);


}

void pwm_duty_fade_change(const ledc_channel_config_t *pwm_channel,uint target_duty,uint max_fade_time_ms)  {
    // Initialize fade service.
    //esp_err_t err_code = ledc_fade_func_install(0);  //占空值逐渐变
    printf("PWM channel %2d fade up to duty = %2d\n",pwm_channel->channel,target_duty);

    ledc_set_fade_with_time(pwm_channel->speed_mode,pwm_channel->channel, target_duty, max_fade_time_ms);
    ledc_fade_start(pwm_channel->speed_mode,pwm_channel->channel, LEDC_FADE_NO_WAIT);
    //ledc_set_fade_time_and_start(pwm_channel->speed_mode,pwm_channel->channel, target_duty, max_fade_time_ms,LEDC_FADE_NO_WAIT);
}

void pwm_duty_change(const ledc_channel_config_t *pwm_channel,uint target_duty){

    printf("PWM channel %2d duty is= %2d\n",pwm_channel->channel,target_duty);

    ledc_set_duty(pwm_channel->speed_mode,pwm_channel->channel, target_duty);
    ledc_update_duty(pwm_channel->speed_mode,pwm_channel->channel);
}
