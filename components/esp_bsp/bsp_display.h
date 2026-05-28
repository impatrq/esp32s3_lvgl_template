#ifndef __BSP_DISPLAY_H__
#define __BSP_DISPLAY_H__

#include <stdio.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

/* Board display rotation: 0, 90, 180, or 270 */
#define BSP_DISPLAY_ROTATION      (90)

/* Native panel resolution (portrait) */
#define BSP_LCD_NATIVE_H_RES      (172)
#define BSP_LCD_NATIVE_V_RES      (320)

/* Effective resolution after rotation */
#if BSP_DISPLAY_ROTATION == 90 || BSP_DISPLAY_ROTATION == 270
#define BSP_LCD_H_RES             BSP_LCD_NATIVE_V_RES
#define BSP_LCD_V_RES             BSP_LCD_NATIVE_H_RES
#else
#define BSP_LCD_H_RES             BSP_LCD_NATIVE_H_RES
#define BSP_LCD_V_RES             BSP_LCD_NATIVE_V_RES
#endif

#define BSP_LCD_DRAW_BUFF_HEIGHT  (50)
#define BSP_LCD_DRAW_BUFF_DOUBLE  (1)

/* SPI bus */
#define BSP_SPI_HOST              SPI2_HOST
#define BSP_LCD_PIXEL_CLOCK_HZ    (80 * 1000 * 1000)

/* GPIO pins */
#define BSP_PIN_LCD_MISO          GPIO_NUM_NC
#define BSP_PIN_LCD_MOSI          GPIO_NUM_39
#define BSP_PIN_LCD_SCLK          GPIO_NUM_38
#define BSP_PIN_LCD_CS            GPIO_NUM_21
#define BSP_PIN_LCD_DC            GPIO_NUM_45
#define BSP_PIN_LCD_RST           GPIO_NUM_40
#define BSP_PIN_LCD_BL            GPIO_NUM_46

/* Backlight PWM */
#define LCD_BL_LEDC_TIMER         LEDC_TIMER_0
#define LCD_BL_LEDC_MODE          LEDC_LOW_SPEED_MODE
#define LCD_BL_LEDC_CHANNEL       LEDC_CHANNEL_0
#define LCD_BL_LEDC_DUTY_RES      LEDC_TIMER_10_BIT
#define LCD_BL_LEDC_DUTY          (1024)
#define LCD_BL_LEDC_FREQUENCY     (5000)

#ifdef __cplusplus
extern "C" {
#endif

void bsp_display_init(void);
void bsp_display_brightness_init(void);
void bsp_display_set_brightness(uint8_t brightness);
uint8_t bsp_display_get_brightness(void);

esp_lcd_panel_io_handle_t bsp_display_get_io_handle(void);
esp_lcd_panel_handle_t bsp_display_get_panel_handle(void);

#ifdef __cplusplus
}
#endif

#endif
