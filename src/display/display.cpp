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
lv_style_t btnm_style;
lv_style_t btnm_btn_style;

const char *btnm_map_20[] PROGMEM_DISPLAY_BTNM = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "\n", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", ""};
int last_id = -1;

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

        // Serial.println(last_x);
        // Serial.println(last_y);
        // Serial.println();
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

    lv_style_init(&btnm_style);
    lv_style_set_pad_all(&btnm_style, 0);
    lv_style_set_pad_gap(&btnm_style, 0);
    lv_style_set_clip_corner(&btnm_style, true);
    lv_style_set_border_width(&btnm_style, 0);

    lv_style_init(&btnm_btn_style);
    lv_style_set_radius(&btnm_btn_style, 0);
    lv_style_set_border_width(&btnm_btn_style, 1);
    lv_style_set_border_opa(&btnm_btn_style, LV_OPA_50);
    lv_style_set_border_color(&btnm_btn_style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_border_side(&btnm_btn_style, LV_BORDER_SIDE_INTERNAL);
    lv_style_set_radius(&btnm_btn_style, 0);
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

void btnm_cb(struct _lv_event_t *event)
{
    uint16_t id = lv_btnmatrix_get_selected_btn(event->current_target);
    Serial.println(id);

    // btnm_ext_t *btnm_ext = (btnm_ext_t *)lv_obj_get_user_data(event->current_target);
    // Serial.println(btnm_ext->last_active);
    
    // uint8_t text = (uint8_t)atoi(lv_btnmatrix_get_btn_text(event->current_target, id));
    // Serial.println(text);

    if (event->code == LV_EVENT_VALUE_CHANGED) {
        if (id == last_id) { // disconnect
            Serial.println("DISCONNECT");
        } else { // connect
            Serial.println("CONNECT");
        }
    }
    if (event->code == LV_EVENT_RELEASED || event->code == LV_EVENT_PRESS_LOST) {
        if (id == last_id) { // uncheck
            Serial.println("UNCKECK");
            lv_btnmatrix_clear_btn_ctrl(event->current_target, id, LV_BTNMATRIX_CTRL_CHECKED);
            last_id = -1;
        } else {
            last_id = id;
            Serial.print("SET LAST ACTIVE: ");
            Serial.println(id);
        }
    }
}

PROGMEM_DISPLAY void Display::render()
{
    Serial.println("Display render...");

    lv_obj_t *pg = lv_obj_create(NULL);

    // BASIC FUNCTIONALITY
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

    // DEBUG CHECKABLE
    // lv_obj_t *pad_pg = lv_obj_create(pg);
    // lv_obj_set_scrollbar_mode(pad_pg, LV_SCROLLBAR_MODE_OFF);
    // lv_obj_clear_flag(pad_pg, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_size(pad_pg, CTRL_WIDTH, CTRL_HEIGHT);
    // lv_obj_align(pad_pg, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_add_style(pad_pg, &ctrl_style, LV_PART_MAIN);

    // lv_obj_t *label = lv_label_create(pad_pg);
    // lv_label_set_text(label, "Name");
    // lv_obj_align(label, LV_ALIGN_TOP_LEFT, 6, 8);

    // lv_obj_t *btn = lv_btn_create(pad_pg);
    // lv_obj_set_size(btn, ROTARY_WIDTH - 3, ROTARY_HEIGHT);
    // lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, LABEL_HEIGHT);
    // lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    // ctrl_btn_ext_t *btn_ext = (ctrl_btn_ext_t *)malloc(sizeof(ctrl_btn_ext_t));
    // btn_ext->pad = pad_pg;
    // lv_obj_set_user_data(btn, btn_ext);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESSED, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESSING, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_RELEASED, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_PRESS_LOST, NULL);
    // lv_obj_add_event_cb(btn, ctrl_btn_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // lv_obj_t *patch = lv_btn_create(pad_pg);
    // lv_obj_set_size(patch, ROTARY_WIDTH - 3, ROTARY_BUTTON_HEIGHT);
    // lv_obj_align(patch, LV_ALIGN_TOP_LEFT, 0, LABEL_HEIGHT + ROTARY_HEIGHT - 1);
    // lv_obj_set_style_pad_top(patch, 2, LV_PART_MAIN);
    // lv_obj_add_flag(patch, LV_OBJ_FLAG_CHECKABLE);

    // DEBUG BTNM CTRL STATE
    lv_obj_t *btnm = lv_btnmatrix_create(pg);
    lv_btnmatrix_set_map(btnm, btnm_map_20);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_one_checked(btnm, true);
    lv_obj_set_size(btnm, LV_HOR_RES, 2 * BUTTON_HEIGHT + 10);
    lv_obj_align(btnm, LV_ALIGN_TOP_LEFT, 0, LABEL_HEIGHT + 18);
    lv_obj_add_style(btnm, &btnm_style, 0);
    lv_obj_add_style(btnm, &btnm_btn_style, LV_PART_ITEMS);
    lv_obj_add_event_cb(btnm, btnm_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(btnm, btnm_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btnm, btnm_cb, LV_EVENT_PRESS_LOST, NULL);
    // btnm_ext_t *btnm_ext = (btnm_ext_t *)malloc(sizeof(btnm_ext_t));
    // btnm_ext->last_active = -1; // init to empty patch
    // lv_obj_set_user_data(btnm, btnm_ext);
    lv_btnmatrix_set_selected_btn(btnm, 0);
    
    Serial.println(lv_btnmatrix_get_selected_btn(btnm)); // SANITY CHECK

    lv_scr_load(pg);
    Serial.println("complete!");
}

Display DC = Display();

} // namespace display