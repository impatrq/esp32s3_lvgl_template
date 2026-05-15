#include "bsp_display.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"

#include "esp_lcd_jd9853.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

static const char *TAG = "bsp_display";

static esp_lcd_panel_io_handle_t s_io_handle = NULL;
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static uint8_t s_brightness = 0;

void bsp_display_init(void)
{
    ESP_LOGI(TAG, "SPI BUS init");
    spi_bus_config_t buscfg = {};
    buscfg.sclk_io_num = BSP_PIN_LCD_SCLK;
    buscfg.mosi_io_num = BSP_PIN_LCD_MOSI;
    buscfg.miso_io_num = BSP_PIN_LCD_MISO;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = BSP_LCD_H_RES * BSP_LCD_DRAW_BUFF_HEIGHT;
    ESP_ERROR_CHECK(spi_bus_initialize(BSP_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config = JD9853_PANEL_IO_SPI_CONFIG(BSP_PIN_LCD_CS, BSP_PIN_LCD_DC, NULL, NULL);
    io_config.pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_SPI_HOST, &io_config, &s_io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BSP_PIN_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    esp_lcd_new_panel_jd9853(s_io_handle, &panel_config, &s_panel_handle);

    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(s_panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(s_panel_handle, false, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_panel_handle, true));
}

esp_lcd_panel_io_handle_t bsp_display_get_io_handle(void)
{
    return s_io_handle;
}

esp_lcd_panel_handle_t bsp_display_get_panel_handle(void)
{
    return s_panel_handle;
}

void bsp_display_brightness_init(void)
{
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode = LCD_BL_LEDC_MODE;
    ledc_timer.timer_num = LCD_BL_LEDC_TIMER;
    ledc_timer.duty_resolution = LCD_BL_LEDC_DUTY_RES;
    ledc_timer.freq_hz = LCD_BL_LEDC_FREQUENCY;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {};
    ledc_channel.speed_mode = LCD_BL_LEDC_MODE;
    ledc_channel.channel = LCD_BL_LEDC_CHANNEL;
    ledc_channel.timer_sel = LCD_BL_LEDC_TIMER;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.gpio_num = BSP_PIN_LCD_BL;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void bsp_display_set_brightness(uint8_t brightness)
{
    if (brightness > 100) {
        brightness = 100;
        ESP_LOGE(TAG, "Brightness value out of range");
    }
    s_brightness = brightness;
    uint32_t duty = (brightness * (LCD_BL_LEDC_DUTY - 1)) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LCD_BL_LEDC_MODE, LCD_BL_LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LCD_BL_LEDC_MODE, LCD_BL_LEDC_CHANNEL));
    ESP_LOGI(TAG, "LCD brightness set to %d%%", brightness);
}

uint8_t bsp_display_get_brightness(void)
{
    return s_brightness;
}
