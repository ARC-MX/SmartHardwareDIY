#ifndef _NVS_VALUE_H_
#define _NVS_VALUE_H_



#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"


#ifdef __cplusplus
extern "C"{
#endif


//#define 


#ifdef __cplusplus
}
#endif


#define PWM_STORAGE                     "PWM_DUTY"
#define BLUETOOTH_DEVICE_NAME_STORAGE   "NAME"

using namespace std;
void nvs_init();
char* nvs_read(string name);
void nvs_write(string name, string value);


#endif
