/*
LVGLDemo

created 17 Apr 2023
by Leonardo Cavagnis
*/

/***************************************************************************************
* Description:  Demonstration using the LVGL library to display & interact with graphics
* Sketch:       LVGLDemo.ino
* Version:      1.0
* Version Desc: Changes to the orginal code by the author above
*               Replaced the bar with an arc in bottom right grid
*               Added and extra button in top right grid
*               Interrogate the states of each input device and output to the serial monitor
*               Adjust screen brightness using slider in bottom left grid
*               Added a switch and label
* Board:        Arduino GIGA R1
* Author:       Steve Fuller
* Website:      sgfpcb@gmail.com
* Comments      https://lvgl.io/docs/latest/en/html/intro/index.html
***************************************************************************************/

#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include <Arduino_GigaDisplay.h>
#include "globals.h"
#include "lvgl.h"

//Create backlight object
GigaDisplayBacklight backlight;

Arduino_H7_Video          Display(800, 480, GigaDisplayShield); /* Arduino_H7_Video Display(1024, 768, USBCVideo); */
Arduino_GigaDisplayTouch  TouchDetector;

bool state = false;

/*******************************************************************
* Call Back Functions from graphic elements
*******************************************************************/

/* Button1 click event callback */
static void btn_event_cb(lv_event_t * e) {
  static uint32_t cnt = 1;
  lv_obj_t * btn = lv_event_get_target(e);
  lv_obj_t * label = lv_obj_get_child(btn, 0);
  lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
  cnt++;
}

/* Button2 click event callback */
/*static void btn2_event_cb(lv_event_t * e) {
  static uint32_t cnt = 1;
  lv_obj_t * btn = lv_event_get_target(e);
  lv_obj_t * label = lv_obj_get_child(btn, 0);
  lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
  cnt++;
}
*/

/* Check buttons callback */
static void cb_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if(code == LV_EVENT_VALUE_CHANGED) {
    const char * txt = lv_checkbox_get_text(obj);
    const char * state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
    Serial.print("txt: "); Serial.print(txt); Serial.print(" state: "); Serial.println(state);
  }
}

/* Slider call back */
static void slider_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if(code == LV_EVENT_VALUE_CHANGED) {
    int32_t value = lv_slider_get_value(obj);
    char buf[12];
    Serial.print(" slider value: "); Serial.println(value);
    setBrightness(value); 
  }
}

/* Arc call back */
static void arc_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if(code == LV_EVENT_VALUE_CHANGED) {
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%dC", (int)lv_arc_get_value(obj)); 
    Serial.print("arc value: "); Serial.println(buf);
  }
}

/* Switch call back */
static void sw1_event_cb(lv_event_t * e){
  static bool sw = false;
  if (sw == 1 ? sw = false : sw = true);
  Serial.print("Switch Condition: "); Serial.println(sw);
}

/* set screen brightness based on slider value */
void setBrightness(int bright){
  backlight.set(bright);
}

/***********************************************************************
* Set up routine
************************************************************************/
void setup() {
  Serial.begin(115200);

  Display.begin();
  TouchDetector.begin();
  backlight.begin();
  setBrightness(BRIGHTNESS);

  /*********************************************************************
  * Create a container with grid 2 cols x 2 rows    [   ] [   ]
  * this container is the whole screen              [   ] [   ]
  *********************************************************************/
  static lv_coord_t col_dsc[] = {370, 370, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {215, 215, LV_GRID_TEMPLATE_LAST};
  lv_obj_t * cont = lv_obj_create(lv_scr_act());
  lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
  lv_obj_set_size(cont, Display.width(), Display.height());
  lv_obj_set_style_bg_color(cont, lv_color_hex(ARDUINO), LV_PART_MAIN);
  lv_obj_center(cont);

  lv_obj_t * label;
  lv_obj_t * obj;

  /**************************************************************************************
  * [0;0] - Top Left
  * Image 
  ***************************************************************************************/
  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
  lv_obj_set_style_bg_color (obj, lv_color_hex(GRID), LV_PART_MAIN);

  LV_IMG_DECLARE(img_arduinologo);
  lv_obj_t * img1 = lv_img_create(obj);
  lv_img_set_src(img1, &img_arduinologo);
  lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_size(img1, 200, 150);

  /************************************************************************************** 
  * [1;0] - Top Right
  * Checkboxes and buttons
  ***************************************************************************************/
  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
  lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_bg_color (obj, lv_color_hex(GRID), LV_PART_MAIN);

  lv_obj_t * cb;
  cb = lv_checkbox_create(obj);
  lv_obj_set_style_text_color (cb,lv_color_hex(WHITE), NULL);
  lv_checkbox_set_text(cb, "Apple");
  lv_obj_add_state(cb, LV_STATE_DEFAULT);
  lv_obj_add_event_cb(cb, cb_event_handler, LV_EVENT_ALL, NULL);

  cb = lv_checkbox_create(obj);
  lv_obj_set_style_text_color (cb,lv_color_hex(WHITE), NULL);
  lv_checkbox_set_text(cb, "Banana");
  lv_obj_add_state(cb, LV_STATE_CHECKED);
  lv_obj_add_event_cb(cb, cb_event_handler, LV_EVENT_ALL, NULL);

  static lv_style_t style_radio;
  static lv_style_t style_radio_chk;
  lv_style_init(&style_radio);
  lv_style_set_radius(&style_radio, LV_RADIUS_CIRCLE);
  lv_style_init(&style_radio_chk);
  lv_style_set_bg_img_src(&style_radio_chk, NULL);

/*
  cb = lv_checkbox_create(obj);
  lv_obj_set_style_text_color (cb,lv_color_hex(WHITE), NULL);
  lv_checkbox_set_text(cb, "Lemon");
  lv_obj_add_flag(cb, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_add_style(cb, &style_radio, LV_PART_INDICATOR);
  lv_obj_add_style(cb, &style_radio_chk, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_add_event_cb(cb, cb_event_handler, LV_EVENT_ALL, NULL);
*/

/*
  cb = lv_checkbox_create(obj);
  lv_obj_set_style_text_color (cb,lv_color_hex(WHITE), NULL);
  lv_checkbox_set_text(cb, "Melon");
  lv_obj_add_flag(cb, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_add_style(cb, &style_radio, LV_PART_INDICATOR);
  lv_obj_add_style(cb, &style_radio_chk, LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_add_state(cb, LV_STATE_CHECKED);
  lv_obj_add_event_cb(cb, cb_event_handler, LV_EVENT_ALL, NULL);
*/
  lv_obj_t * btn = lv_btn_create(obj);
  lv_obj_set_size(btn, 100, 40);
  lv_obj_center(btn);
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, "Click Me 1");
  lv_obj_center(label);

/*
  lv_obj_t * btn2 = lv_btn_create(obj);
  lv_obj_set_size(btn2, 100, 40);
  lv_obj_center(btn2);
  lv_obj_set_pos(btn2, 10, 20);                            
  lv_obj_add_event_cb(btn2, btn2_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn2);
  lv_label_set_text(label, "Click Me 2");
  lv_obj_center(label);
*/

  lv_obj_t * sw1 = lv_switch_create(obj);
  lv_obj_align(sw1, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_event_cb(sw1, sw1_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(obj);
  lv_label_set_text(label, "OFF ON");
  lv_obj_center(label);
  lv_obj_set_style_text_color (label,lv_color_hex(WHITE), NULL);

  /******************************************************************************
  * [0;1] - Bottom Left
  * Slider 
  *******************************************************************************/
  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
  lv_obj_set_style_bg_color (obj, lv_color_hex(GRID), LV_PART_MAIN); // background colour of grid
  
  lv_obj_t * slider = lv_slider_create(obj);
  lv_slider_set_range(slider, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  lv_slider_set_value(slider, BRIGHTNESS, LV_ANIM_OFF);
  lv_obj_center(slider);
  lv_obj_set_style_bg_color(slider, lv_color_hex(WHITE), LV_PART_INDICATOR); // color was WHITE

  label = lv_label_create(obj);
  lv_obj_set_style_text_color (label,lv_color_hex(WHITE), NULL);
  lv_label_set_text(label, "Brightness");
  lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
  lv_obj_add_event_cb(slider, slider_event_handler, LV_EVENT_ALL, NULL);
  
  /********************************************************************************
  * [1;1] - Bottom Right
  * Horizontal Bar
  *********************************************************************************/

  // create the grid container
  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
  lv_obj_set_style_text_color (obj,lv_color_hex(WHITE), NULL);
  lv_obj_set_style_bg_color (obj, lv_color_hex(GRID), LV_PART_MAIN);

  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(obj);
  lv_obj_set_size(arc, 150, 150);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_arc_set_value(arc, 40);
  lv_obj_center(arc);
  label = lv_label_create(obj);
  lv_label_set_text(label, "20*C");
  lv_obj_align_to(label, arc, LV_ALIGN_CENTER, 0, 10);
  lv_obj_add_event_cb(arc, arc_event_handler, LV_EVENT_ALL, NULL);

/*
  lv_obj_t * bar = lv_bar_create(obj);
  lv_obj_set_size(bar, 200, 20);
  lv_obj_center(bar);
  lv_bar_set_value(bar, 70, LV_ANIM_OFF);
*/
  /*lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, set_slider_val);
  lv_anim_set_time(&a, 3000);
  lv_anim_set_playback_time(&a, 3000);
  lv_anim_set_var(&a, bar);
  lv_anim_set_values(&a, 0, 100);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_start(&a);*/
}

/**********************************************************************
* Main Program Loop
***********************************************************************/

void loop() { 
  /* Feed LVGL engine */
  lv_timer_handler();
}