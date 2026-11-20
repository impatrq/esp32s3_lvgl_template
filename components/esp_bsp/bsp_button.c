#include "bsp_button.h"
#include "button_gpio.h"
#include "esp_log.h"

static const char *TAG = "bsp_button";
static button_handle_t s_btn_handle = NULL;

esp_err_t bsp_button_init(void)
{
    if (s_btn_handle != NULL) {
        return ESP_OK;
    }

    const button_config_t btn_cfg = {0};
    const button_gpio_config_t gpio_cfg = {
        .gpio_num = BSP_BUTTON_BOOT_GPIO,
        .active_level = 0,
    };

    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &s_btn_handle);
    if (ret != ESP_OK || s_btn_handle == NULL) {
        ESP_LOGE(TAG, "Failed to init GPIO%d button: %s",
                 BSP_BUTTON_BOOT_GPIO, esp_err_to_name(ret));
        return ret != ESP_OK ? ret : ESP_FAIL;
    }

    ESP_LOGI(TAG, "BOOT button initialized on GPIO%d", BSP_BUTTON_BOOT_GPIO);
    return ESP_OK;
}

esp_err_t bsp_button_register_cb(button_event_t event, button_cb_t cb, void *usr_data)
{
    if (s_btn_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return iot_button_register_cb(s_btn_handle, event, NULL, cb, usr_data);
}
