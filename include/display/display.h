/* 
 * File:   display.h
 * Author: Adam Martini
 *
 * Created on May 10, 2020, 11:32 PM
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <lvgl.h>
#include <ILI9488_t3.h>
#include <Adafruit_FT6206.h>


namespace display
{

#define PROGMEM_DISPLAY __attribute__((section(".progmem.disp")))
#define PROGMEM_DISPLAY_BTNM __attribute__((section(".progmem.disp.btn")))
#define PROGMEM_DISPLAY_TXT __attribute__((section(".progmem.disp.txt")))

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST -1
#define TFT_SPI_SPEED 24000000
#define TFT_HOR_RES 480
#define TFT_VER_RES 320

#define TS_CS 32 // on 32 because pin 8 conflicts with audio shield
// #define TS_MINX 3800
// #define TS_MAXX 100
// #define TS_MINY 100
// #define TS_MAXY 3750
#define TS_MINX 0
#define TS_MAXX TFT_VER_RES
#define TS_MINY 0
#define TS_MAXY TFT_HOR_RES
#define TS_MAXZ 100 // indicates least amount of pressure
#define TS_MINZ 10

// #define XYZ_PAD_MINX 300
// #define XYZ_PAD_MAXX 3600
// #define XYZ_PAD_MINY 3100
// #define XYZ_PAD_MAXY 400

#define XYZ_PAD_MINX 0
#define XYZ_PAD_MAXX 470
#define XYZ_PAD_MINY 25
#define XYZ_PAD_MAXY 220

#define BUFFER_LINE_COUNT 10

#define LABEL_HEIGHT 30
#define BUTTON_WIDTH 40
#define BUTTON_HEIGHT 40
#define ROTARY_WIDTH 82
#define ROTARY_HEIGHT 82
#define ROTARY_BUTTON_HEIGHT (BUTTON_HEIGHT - 18)
#define CTRL_WIDTH ROTARY_WIDTH
#define CTRL_HEIGHT 135
#define SLIDER_HEIGHT 24

#define LV_CONTROL_MIN -32768.0f
#define LV_CONTROL_ZERO 0.0f
#define LV_CONTROL_MAX 32767.0f
#define GRANULAR_CONTROL_MIN -1073741824.0f // -536870912.0f  // 30 bits of precision
#define GRANULAR_CONTROL_ZERO 0.0f
#define GRANULAR_CONTROL_MAX 1073741823.0f 

#define ZERO_COUNT_THRESHOLD 6
#define SENSITIVITY_SHIFT 4
#define CONTROL_SLEW_RATE 100000

#define COLOR_BG lv_color_hex(0x586273)
#define COLOR_BG_TEXT lv_color_hex(0xffffff)
#define COLOR_BG_BORDER lv_color_hex(0x2f3237) // lv_color_hex(0x808a97)
#define COLOR_BG_BORDER_ACTIVE lv_color_hex(0xf06292) // purple 0x9575cd
#define COLOR_SCR_TEXT lv_color_hex(0xe7e9ec) // lv_palette_lighten(LV_PALETTE_GREY, 10) // lv_color_hex(0xe7e9ec)
#define COLOR_BTN_DIS lv_color_hex3(0x888)
#define COLOR_BORDER_DARK lv_color_hex(0x3b3e42)
// #define COLOR_INDIC_GREEN_LIGHT lv_color_hex(0xa5d6a7)
#define COLOR_INDIC_GREEN lv_color_hex(0x81c784)
#define COLOR_INDIC_GREEN_YELLOW lv_color_hex(0xaed581)
#define COLOR_INDIC_YELLOW_GREEN lv_color_hex(0xdce775)
#define COLOR_INDIC_YELLOW lv_color_hex(0xfff176)
#define COLOR_INDIC_YELLOW_ORANGE lv_color_hex(0xffd54f)
#define COLOR_INDIC_ORANGE lv_color_hex(0xffb74d)
#define COLOR_INDIC_ORANGE_RED lv_color_hex(0xff8a65)
// #define COLOR_INDIC_RED_LIGHT lv_color_hex(0xef9a9a)
#define COLOR_INDIC_RED lv_color_hex(0xaf4448)
#define COLOR_INDIC_PURPLE lv_color_hex(0x883997)

#define BORDER_WIDTH LV_DPX(2)
#define PADDING LV_DPX(15)

#define DISPLAY_NUM_CTRL 150

extern lv_color_t buf_1[TFT_HOR_RES * BUFFER_LINE_COUNT];
extern lv_color_t buf_2[TFT_HOR_RES * BUFFER_LINE_COUNT];

typedef struct ctrl_btn_ext_t
{
    lv_obj_t *pad;
} ctrl_btn_ext_t;

class Display
{
    lv_disp_draw_buf_t disp_buf;
    lv_disp_drv_t disp_drv;
    lv_disp_t *disp;

    lv_indev_drv_t indev_drv_ts;

public:
    Display();

    void begin();
    void render();

    inline void update() __attribute__((always_inline));
};

void Display::update() {}

extern Display DC;

} // namespace display

#endif /* DISPLAY_H */