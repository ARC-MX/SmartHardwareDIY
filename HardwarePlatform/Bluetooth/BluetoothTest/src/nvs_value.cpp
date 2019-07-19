#include "nvs_value.h"


void nvs_init(){
    
    /* Initialize NVS — it is used to store PHY calibration data */

    //定义错误代码的字符串  非易失性存储（NVS）初始化
    esp_err_t err = nvs_flash_init(); 
    //如果NVS分区被截断，可能会发生这种情况。擦除整个分区并再次调用nvs flash init。
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {//如果NVS分区不包含任何空页。
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    //如果传给 ESP_ERROR_CHECK() 的参数不等于 ESP_OK ，则会在控制台上打印错误消息，然后调用 abort() 函数。
    ESP_ERROR_CHECK( err );

}

char* nvs_read(std::string name){
    nvs_handle nhandle;
    esp_err_t err = nvs_open(name.c_str(), NVS_READWRITE, &nhandle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
        // Read
        printf("Reading value of %s from NVS ... ",name.c_str());
        size_t required_size;
        nvs_get_str(nhandle, name.c_str(), NULL, &required_size);
        char* value = (char *)malloc(required_size);

        nvs_get_str(nhandle, name.c_str(), value, &required_size);

        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Reading value = %s\n", value);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
        // Close
        nvs_close(nhandle);
        return value;
    }
    return _NULL;
}

void nvs_write(string name, string value)
{

    nvs_handle nhandle;
    esp_err_t err = nvs_open(name.c_str(), NVS_READWRITE, &nhandle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
        printf("Write value of %s = %s into NVS ... ",name.c_str(),value.c_str());

        err = nvs_set_str(nhandle, name.c_str(), value.c_str());
        assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(nhandle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    }
    // Close
    nvs_close(nhandle);

}