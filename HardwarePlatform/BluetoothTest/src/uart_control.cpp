/*
* @file         uart.c 
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
#include "uart_control.h"

/* 
===========================
全局变量定义
=========================== 
*/

static const char *TAG = "uart_example";
QueueHandle_t uart0_queue;

//串口0任务创建
void uart_task(void *pvParameters)
{
    int uart_num = (int) pvParameters;
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(BUF_SIZE);
    while(1) {
        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void *)&event, (portTickType)portMAX_DELAY)) {
            ESP_LOGI(TAG, "uart[%d] event:", uart_num);
            switch(event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.
                in this example, we don't process data in event, but read data outside.*/
                case UART_DATA:
                    uart_get_buffered_data_len((uart_port_t)uart_num, &buffered_size);
                    ESP_LOGI(TAG, "data, len: %d; buffered len: %d", event.size, buffered_size);


                    // /*bluetooth ****************************************************************/
                    // if (event.size && (is_connect == true) && (db != NULL) && ((db+SPP_IDX_SPP_DATA_RECV_VAL)->properties & (ESP_GATT_CHAR_PROP_BIT_WRITE_NR | ESP_GATT_CHAR_PROP_BIT_WRITE))) {
                    //     uint8_t * temp = NULL;
                    //     temp = (uint8_t *)malloc(sizeof(uint8_t)*event.size);
                    //     if(temp == NULL){
                    //         ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n", __func__, __LINE__);
                    //         break;
                    //     }
                    //     memset(temp, 0x0, event.size);
                    //     uart_read_bytes(UART_NUM_0,temp,event.size,portMAX_DELAY);
                    //     esp_ble_gattc_write_char( spp_gattc_if,
                    //                             spp_conn_id,
                    //                             (db+SPP_IDX_SPP_DATA_RECV_VAL)->attribute_handle,
                    //                             event.size,
                    //                             temp,
                    //                             ESP_GATT_WRITE_TYPE_RSP,
                    //                             ESP_GATT_AUTH_REQ_NONE);
                    //     free(temp);
                    // }
                    
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow\n");
                    //If fifo overflow happened, you should consider adding flow control for your application.
                    //We can read data out out the buffer, or directly flush the rx buffer.
                    uart_flush((uart_port_t)uart_num);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full\n");
                    //If buffer full happened, you should consider encreasing your buffer size
                    //We can read data out out the buffer, or directly flush the rx buffer.
                    uart_flush((uart_port_t)uart_num);
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break\n");
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error\n");
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error\n");
                    break;
                //UART_PATTERN_DET
                case UART_PATTERN_DET:
                    ESP_LOGI(TAG, "uart pattern detected\n");
                    break;
                //Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d\n", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

//an example of echo test with hardware flow control on UART1
void uart_echo()
{
    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
	
    while(1) {
        //Read data from UART
		int len0 = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 100 / portTICK_RATE_MS);
		//Write data back to UART
		if(len0 > 0) {
            ESP_LOGI(TAG, "uart0 read : %d", len0);
            uart_write_bytes(UART_NUM_0, (const char*)data, len0);
        }

		int len1 = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
		if(len1 > 0) {
            ESP_LOGI(TAG, "uart1 read : %d", len1);
            uart_write_bytes(UART_NUM_1, (const char*)data, len1);
        }

    }
	free(data);
}

/*
* esp32双路串口配置
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
void uart_init(void)
{
	uart0_init();
	uart1_init();
}

/*
* esp32串口0配置
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    ARCW, 2018/08/06, 初始化版本\n 
*/
void uart0_init(){
    //串口配置结构体
	uart_config_t uart0_config;
	//串口参数配置->uart1
	uart0_config.baud_rate = 115200;					//波特率
	uart0_config.data_bits = UART_DATA_8_BITS;			//数据位
	uart0_config.parity = UART_PARITY_DISABLE;			//校验位
	uart0_config.stop_bits = UART_STOP_BITS_1;			//停止位
	uart0_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;	//硬件流控
	uart0_config.rx_flow_ctrl_thresh=122;
	ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart0_config));//设置串口

    //Set UART pins,(-1: default pin, no change.)
    //For UART0, we can just use the default pins.
    //uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    //Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);
    //Install UART driver, and get the queue.
	//注册串口服务即使能+设置缓存区大小
	uart_driver_install(UART_NUM_0, RX0_BUF_SIZE * 2, TX0_BUF_SIZE * 2, 10, &uart0_queue, 0);
    //Set uart pattern detect function.
	uart_enable_pattern_det_intr(UART_NUM_0, '+', 3, 10000, 10, 10);

}
/*
* esp32串口1配置
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    ARCW, 2018/08/06, 初始化版本\n 
*/
void uart1_init(){
    //串口配置结构体
	uart_config_t uart1_config;
	//串口参数配置->uart1
	uart1_config.baud_rate = 115200;					//波特率
	uart1_config.data_bits = UART_DATA_8_BITS;			//数据位
	uart1_config.parity = UART_PARITY_DISABLE;			//校验位
	uart1_config.stop_bits = UART_STOP_BITS_1;			//停止位
	uart1_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;	//硬件流控
	ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart1_config));//设置串口
	//IO映射-> T:IO4  R:IO5
	uart_set_pin(UART_NUM_1, TXD1_PIN, RXD1_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	//注册串口服务即使能+设置缓存区大小
	uart_driver_install(UART_NUM_1, RX1_BUF_SIZE * 2, 0, 0, NULL, 0);

	// if (Interrupt_Flag){
	// 	//这里是关键点，必须要先uart_driver_install安装驱动，在把中断服务给释放掉
	// 	printf("设置中断");
	// 	uart_isr_free(UART_NUM_1);
	// 	//重新注册中断服务函数
	// 	uart_isr_handle_t handle;
	// 	uart_isr_register(UART_NUM_1, uart1_irq_handler, NULL, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM, &handle);
	// 	//使能串口接收中断
	// 	uart_enable_rx_intr(UART_NUM_1);
	// }

}
// void spp_uart_init(void)
// {
//     uart_config_t uart_config = {
//         .baud_rate = 115200,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_RTS,
//         .rx_flow_ctrl_thresh = 122,
//     };

//     //Set UART parameters
//     uart_param_config(UART_NUM_0, &uart_config);
//     //Set UART pins
//     uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
//     //Install UART driver, and get the queue.
//     uart_driver_install(UART_NUM_0, 4096, 8192, 10, &spp_uart_queue, 0);
//     xTaskCreate(uart_task, "uTask", 2048, (void*)UART_NUM_0, 8, NULL);
// }
// //中断服务函数放在IRAM中执行，所以在这里定义的时候要添加IRAM_ATTR
// void IRAM_ATTR uart1_irq_handler(void *arg) {
//     volatile uart_dev_t *uart = &UART1;
//     uart->int_clr.rxfifo_full = 1;
//     uart->int_clr.frm_err = 1;
//     uart->int_clr.rxfifo_tout = 1;
// 	printf("进入中断");
// 	uart_write_bytes(UART_NUM_1, "进入中断", strlen("进入中断"));
//     while (uart->status.rxfifo_cnt) {
		
//         uint8_t c = uart->fifo.rw_byte;
//         uart_write_bytes(UART_NUM_1, (char *)&c, 1);  //把接收的数据重新打印出来
//     }
// }


