#include "display/display.h"

namespace display
{
    
lv_color_t buf_1[TFT_HOR_RES * BUFFER_LINE_COUNT]{};
lv_color_t buf_2[TFT_HOR_RES * BUFFER_LINE_COUNT]{};

namespace
{

ILI9488_t3 tft = ILI9488_t3(TFT_CS, TFT_DC, TFT_RST);
Adafruit_FT6206 ts = Adafruit_FT6206();

lv_indev_t *indev_ts;

float last_x = 0, last_y = 0, last_z = 0; // used for granular slew limiting between TS reads
lv_coord_t last_x_coord = 0;
lv_coord_t last_y_coord = 0;

lv_style_t ctrl_style;

}

void flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int16_t x1 = (int16_t)area->x1;
    int16_t y1 = (int16_t)area->y1;
    int16_t x2 = (int16_t)area->x2;
    int16_t y2 = (int16_t)area->y2;
    int16_t w = x2 - x1 + 1;
    int16_t h = y2 - y1 + 1;

    tft.writeRect(x1, y1, w, h, (uint16_t *)color_p);

    lv_disp_flush_ready(disp_drv); // inform the graphics library flushing complete
}

void input_ts(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    // save the state and save the pressed coordinate
    data->state = ts.touched() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if (data->state == LV_INDEV_STATE_PR) {
        TS_Point p = ts.getPoint(0);

        last_x = (float)p.y;
        last_y = (float)p.x;
        last_z = (float)TS_MINZ;

        last_x_coord = map(p.y, 0, 480, 0, 480);
        last_y_coord = map(p.x, 0, 320, 320, 0);

        Serial.println(last_x);
        Serial.println(last_y);
        Serial.println();
    } else {
        last_x_coord = last_y_coord = -1; // avoid triggering events for last touch point
    }

    // set the coordinates (if released use the last pressed coordinates)
    data->point.x = last_x_coord;
    data->point.y = last_y_coord;
}

Display::Display() {}

PROGMEM_DISPLAY void Display::begin()
{
    tft.begin(TFT_SPI_SPEED);
    tft.setRotation(1);

    if (!ts.begin(40)) Serial.println("Unable to start touchscreen.");

    lv_init();

    // display
    lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, TFT_HOR_RES * BUFFER_LINE_COUNT);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_HOR_RES;
    disp_drv.ver_res = TFT_VER_RES;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = flush;
    disp = lv_disp_drv_register(&disp_drv);

    // input
    lv_indev_drv_init(&indev_drv_ts);
    indev_drv_ts.type = LV_INDEV_TYPE_POINTER;
    indev_drv_ts.read_cb = input_ts;
    indev_ts = lv_indev_drv_register(&indev_drv_ts);

    // styles
    lv_style_init(&ctrl_style);
    lv_style_set_pad_top(&ctrl_style, 0);
    lv_style_set_pad_right(&ctrl_style, 0);
    lv_style_set_pad_bottom(&ctrl_style, 0);
    lv_style_set_pad_left(&ctrl_style, 0);
}

void ctrl_btn_event_cb(lv_event_t *event)
{
    ctrl_btn_ext_t *btn_ext = (ctrl_btn_ext_t *)lv_obj_get_user_data(event->current_target);
    // ctrl_pad_ext_t *pad_ext = (ctrl_pad_ext_t *)lv_obj_get_user_data(btn_ext->pad);
    if (event->code == LV_EVENT_PRESSED) {
        Serial.println("PRESSED");
    }
    if (event->code == LV_EVENT_RELEASED || event->code == LV_EVENT_PRESS_LOST) {
        Serial.println("PRESSING");
    }
    if (event->code == LV_EVENT_VALUE_CHANGED) {
        lv_state_t state = lv_obj_get_state(event->current_target);
        Serial.print("VALUE CHANGED: ");
        if (state == LV_STATE_DEFAULT) {
            Serial.println("DEFAULT");
        } else if (state & LV_STATE_CHECKED) {
            Serial.println("CHECKED");
        }
    }
}

PROGMEM_DISPLAY void Display::render()
{
    Serial.println("Display render...");

    lv_obj_t *pg = lv_obj_create(NULL);

    // lv_obj_t *label = lv_label_create(pg);
    // lv_label_set_text(label, "HELLO");
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // lv_obj_t *btn = lv_btn_create(pg);
    // lv_obj_set_size(btn, ROTARY_WIDTH - 3, ROTARY_HEIGHT);
    // lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESSED, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESSING, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_RELEASED, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESS_LOST, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *pad_pg = lv_obj_create(pg);
    lv_obj_set_scrollbar_mode(pad_pg, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(pad_pg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(pad_pg, CTRL_WIDTH, CTRL_HEIGHT);
    lv_obj_align(pad_pg, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_style(pad_pg, &ctrl_style, LV_PART_MAIN);
    // lv_obj_add_event_cb(pad_pg, ctrl_pad_event_cb, LV_EVENT_PRESSED, NULL);
    // ctrl_pad_ext_t *pad_ext = (ctrl_pad_ext_t *)malloc(sizeof(ctrl_pad_ext_t));
    // pad_ext->ctrl = ctrl;
    // pad_ext->grp_id = cnt % 10;
    // lv_obj_set_user_data(pad_pg, pad_ext);

    lv_obj_t *label = lv_label_create(pad_pg);
    lv_label_set_text(label, "Name");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 6, 8);

    lv_obj_t *btn = lv_btn_create(pad_pg);
    lv_obj_set_size(btn, ROTARY_WIDTH - 3, ROTARY_HEIGHT);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, LABEL_HEIGHT);
    // lv_obj_add_style(btn, &btn_style, 0);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    ctrl_btn_ext_t *btn_ext = (ctrl_btn_ext_t *)malloc(sizeof(ctrl_btn_ext_t));
    btn_ext->pad = pad_pg;
    lv_obj_set_user_data(btn, btn_ext);
    lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESS_LOST, NULL);
    lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *patch = lv_btn_create(pad_pg);
    lv_obj_set_size(patch, ROTARY_WIDTH - 3, ROTARY_BUTTON_HEIGHT);
    lv_obj_align(patch, LV_ALIGN_TOP_LEFT, 0, LABEL_HEIGHT + ROTARY_HEIGHT - 1);
    // lv_obj_add_style(patch, &btn_style, 0);
    // lv_obj_add_style(patch, &no_padding_style, LV_PART_MAIN);
    lv_obj_set_style_pad_top(patch, 2, LV_PART_MAIN);
    lv_obj_add_flag(patch, LV_OBJ_FLAG_CHECKABLE);
    // ctrl_patch_ext_t *patch_ext = (ctrl_patch_ext_t *)malloc(sizeof(ctrl_patch_ext_t));
    // patch_ext->ctrl_module_id = module_id;
    // patch_ext->ctrl_type = type;
    // patch_ext->ctrl_id = ctrl_id;
    // patch_ext->label = NULL;
    // lv_obj_set_user_data(patch, patch_ext);
    // lv_obj_add_event_cb(patch, ctrl_patch_event_cb, LV_EVENT_RELEASED, NULL);
    // lv_obj_add_event_cb(patch, ctrl_patch_event_cb, LV_EVENT_PRESS_LOST, NULL);

    lv_scr_load(pg);
    Serial.println("complete!");
}

Display DC = Display();

} // namespace display