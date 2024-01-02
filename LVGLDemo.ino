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
*               Added and extra button in top right grid. Now commented out
*               Interrogate the states of each input device and output to the serial monitor
*               Adjust screen brightness using slider in bottom left grid
*               Added a switch and label to top right grid
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
    Serial.print("arc value  : "); Serial.println(buf);

    int mValue = map(lv_arc_get_value(obj), ARC_MIN, ARC_MAX, METER_MIN, METER_MAX);
    Serial.print("meter value: "); Serial.println((mValue));
    drawMeter(mValue);
    meterValue = mValue;

    int Brightness = map(mValue, METER_MIN, METER_MAX, ARC_MIN, ARC_MAX);
    setBrightness(mValue);
  }
}

/* Switch call back */
static void sw1_event_cb(lv_event_t * e) {
  static bool sw = false;
  if (sw == 1 ? sw = false : sw = true);
  Serial.print("Switch Condition: "); Serial.println(sw);
}

/* set screen brightness based on slider value */
void setBrightness(int bright){
  backlight.set(bright);
}

// rollers callback function
static void roller_event_cb(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if(code == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_roller_get_selected_str(obj, buf, sizeof(buf));
    Serial.print("Roller value: "); Serial.println(buf);
  }
}

// meter callback function
static void meter_event_cb(lv_event_t * e){
  lv_obj_t * obj = lv_event_get_target(e);
  static int x = 0;
  x++;
  Serial.print("Meter Event"); Serial.println(x);

}

/*********************************************************************************
// draw widget functions
*********************************************************************************/

/**********************************************************************************
* METER WIDGET
* takes integer value and display meter needle on meter dial
***********************************************************************************/
static void drawMeter(int mValue){

  // create a meter object
  static lv_obj_t * meter;

  // create the meter widget
  meter = lv_meter_create(lv_scr_act());
  lv_obj_center(meter);
  lv_obj_align(meter, LV_ALIGN_CENTER, METER_X, METER_Y);
  lv_obj_set_size(meter, METER_WIDTH, METER_HEIGHT);
  lv_obj_add_event_cb(meter, meter_event_cb, LV_EVENT_CLICKED, NULL);
  
  /*Add a visual scale first*/
  lv_meter_scale_t * scale = lv_meter_add_scale(meter);
  lv_meter_set_scale_range(meter, scale, METER_MIN, METER_MAX, 270, 135);
  lv_meter_set_scale_ticks(meter, scale, 41, METER_MINOR_W, METER_MINOR_L, lv_palette_main(LV_PALETTE_GREY));
  lv_meter_set_scale_major_ticks(meter, scale, 10, METER_MAJOR_W, METER_MAJOR_L, lv_color_black(), 10);

  lv_meter_indicator_t * indic;

  /*Add a blue arc to the start*/
  indic = lv_meter_add_arc(meter, scale, METER_MAJOR_W, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_meter_set_indicator_start_value(meter, indic, METER_MIN);
  lv_meter_set_indicator_end_value(meter, indic, METER_COLD);

  /*Make the tick lines blue at the start of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),false, 0);
  lv_meter_set_indicator_start_value(meter, indic, METER_MIN);
  lv_meter_set_indicator_end_value(meter, indic, METER_COLD);

  /*Add a orange arc to the middle*/
  indic = lv_meter_add_arc(meter, scale, METER_MAJOR_W, lv_palette_main(LV_PALETTE_ORANGE), 0);
  lv_meter_set_indicator_start_value(meter, indic, METER_COLD);
  lv_meter_set_indicator_end_value(meter, indic, METER_WARM);

  /*Make the tick lines orange at the middle of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_ORANGE), lv_palette_main(LV_PALETTE_ORANGE),false, 0);
  lv_meter_set_indicator_start_value(meter, indic, METER_COLD);
  lv_meter_set_indicator_end_value(meter, indic, METER_WARM);

  /*Add a red arc to the end*/
  indic = lv_meter_add_arc(meter, scale, METER_MAJOR_W, lv_palette_main(LV_PALETTE_RED), 0);
  lv_meter_set_indicator_start_value(meter, indic, METER_WARM); // 80
  lv_meter_set_indicator_end_value(meter, indic, METER_MAX);  // 100

  /*Make the tick lines red at the end of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
  lv_meter_set_indicator_start_value(meter, indic, METER_WARM);
  lv_meter_set_indicator_end_value(meter, indic, METER_MAX);

  /*Add a needle line indicator*/
  indic = lv_meter_add_needle_line(meter, scale, METER_NEEDLE_W, lv_palette_main(LV_PALETTE_GREY), -10);
  lv_meter_set_indicator_value(meter, indic, mValue);

  /*Create the main label*/
  lv_obj_t * meter_label = lv_label_create(lv_scr_act());
  lv_label_set_text_fmt(meter_label, "%d*C", mValue);
  lv_obj_set_style_text_color (meter_label,lv_color_hex(ARDUINO), NULL);
  
  /*Position the main label*/
  lv_obj_align(meter_label, LV_ALIGN_CENTER, METER_X, METER_Y + 50);

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

  randomSeed(analogRead(0));

  /*********************************************************************
  * Create a container with grid 2 cols x 2 rows    [c0,r0] [c1,r0]
  * this container is the whole screen              [c0,r1] [c1,r1]
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
  static lv_obj_t * meter;

  /**************************************************************************************
  * [c0;r0] - Top Left
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
  * [c1;r0] - Top Right
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

  lv_obj_t * btn = lv_btn_create(obj);
  lv_obj_set_size(btn, 100, 40);
  lv_obj_center(btn);
  lv_obj_align(btn, LV_ALIGN_CENTER, 100, -100);
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, "Click Me 1");
  lv_obj_center(label);

  lv_obj_t * sw1 = lv_switch_create(obj);
  lv_obj_align(sw1, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_event_cb(sw1, sw1_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(obj);
  lv_label_set_text(label, "OFF ON");
  lv_obj_center(label);
  lv_obj_set_style_text_color (label,lv_color_hex(WHITE), NULL);

  /*********************************************************************************
  * CREATE ROLLERS
  **********************************************************************************/

  /*A style to make the selected option larger*/
  static lv_style_t style_sel;
  lv_style_init(&style_sel);
  //lv_style_set_text_font(&style_sel, &lv_font_montserrat_22);

  // set objects and variables for the rollers on screen
  const char * opt_brightness = "TEMP\n10*c\n25*C\n50*C\n75*C\n90*C";
  const char * opt_temp = "SCALE\n*C\n*F\n*K";
  const char * opt_days = "DAYS\nMonday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday";
  lv_obj_t * rollerBrightness;
  lv_obj_t * rollerDays;
  lv_obj_t * rollerTemp;

  /**************************************************************************************
  * Roller Brightness
  **************************************************************************************/
  rollerBrightness = lv_roller_create(lv_scr_act());
  lv_roller_set_options(rollerBrightness, opt_brightness, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(rollerBrightness, BRIGHT_ROWS);
  lv_roller_set_selected(rollerBrightness, BRIGHT_INDEX, LV_ANIM_ON);                           // select the 2nd option
  lv_obj_set_width(rollerBrightness, BRIGHT_WIDTH);
  lv_obj_add_style(rollerBrightness, &style_sel, LV_PART_SELECTED);
  lv_obj_set_style_text_align(rollerBrightness, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_align(rollerBrightness, LV_ALIGN_LEFT_MID, BRIGHT_X, BRIGHT_Y);
  lv_obj_add_event_cb(rollerBrightness, roller_event_cb, LV_EVENT_ALL, NULL);

  /*Create the main label*/
  lv_obj_t * bright_label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_color (bright_label,lv_color_hex(WHITE), NULL);
  lv_label_set_text(bright_label, BRIGHT_LABEL);
  /*Position the main label*/
  lv_obj_align(bright_label, LV_ALIGN_CENTER, BRIGHT_X -350, BRIGHT_Y + 65);

  /***************************************************************************************
  * Temperature Selection Roller
  ***************************************************************************************/
  rollerTemp = lv_roller_create(lv_scr_act());
  lv_roller_set_options(rollerTemp, opt_temp, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(rollerTemp, TEMP_ROWS);
  lv_obj_add_style(rollerTemp, &style_sel, LV_PART_SELECTED);
  lv_obj_align(rollerTemp, LV_ALIGN_CENTER, TEMP_X, TEMP_Y);
  lv_obj_add_event_cb(rollerTemp, roller_event_cb, LV_EVENT_ALL, NULL);
  lv_roller_set_selected(rollerTemp, TEMP_INDEX, LV_ANIM_OFF);

  /*Create the temperature label*/
  lv_obj_t * temp_label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_color (temp_label,lv_color_hex(WHITE), NULL);
  lv_label_set_text(temp_label, TEMP_LABEL);
  /*Position the main label*/
  lv_obj_align(temp_label, LV_ALIGN_CENTER, TEMP_X , TEMP_Y + 65);

  /****************************************************************************************
  * Days Roller
  ******************************************************************************************/
  rollerDays = lv_roller_create(lv_scr_act());
  lv_roller_set_options(rollerDays, opt_days, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(rollerDays, DAYS_ROWS);
  lv_roller_set_selected(rollerDays, DAYS_INDEX, LV_ANIM_ON);                           // select the 2nd option
  lv_obj_set_width(rollerDays, DAYS_WIDTH);
  lv_obj_add_style(rollerDays, &style_sel, LV_PART_SELECTED);
  lv_obj_set_style_text_align(rollerDays, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(rollerDays, LV_ALIGN_CENTER, DAYS_X, DAYS_Y);
  lv_obj_add_event_cb(rollerDays, roller_event_cb, LV_EVENT_ALL, NULL);

  /*Create the main label*/
  lv_obj_t * days_label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_color (days_label,lv_color_hex(WHITE), NULL);
  lv_label_set_text(days_label, DAYS_LABEL);
  /*Position the main label*/
  lv_obj_align(days_label, LV_ALIGN_CENTER, DAYS_X , DAYS_Y + 80);

  // draw the meter widget funtion
  drawMeter(meterValue);

  /******************************************************************************
  * [c0;r1] - Bottom Left
  * Slider 
  *******************************************************************************/
  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
  lv_obj_set_style_bg_color (obj, lv_color_hex(GRID), LV_PART_MAIN); // background colour of grid
  
  lv_obj_t * slider = lv_slider_create(obj);
  lv_slider_set_range(slider, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  lv_slider_set_value(slider, BRIGHTNESS, LV_ANIM_OFF);
  lv_obj_center(slider);
  lv_obj_align(slider, LV_ALIGN_CENTER, SLIDER_X, SLIDER_Y);
  lv_obj_set_style_bg_color(slider, lv_color_hex(WHITE), LV_PART_INDICATOR); // color was WHITE
  lv_obj_add_event_cb(slider, slider_event_handler, LV_EVENT_ALL, NULL);

  label = lv_label_create(obj);
  lv_obj_set_style_text_color (label,lv_color_hex(WHITE), NULL);
  lv_label_set_text(label, SLIDER_LABEL);
  lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

  /********************************************************************************
  * [c1;r1] - Bottom Right
  * Horizontal Bar
  *********************************************************************************/

  // create the grid container
  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
  lv_obj_set_style_text_color (obj,lv_color_hex(WHITE), NULL);
  lv_obj_set_style_bg_color (obj, lv_color_hex(GRID), LV_PART_MAIN);

  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(obj);
  lv_obj_set_size(arc, ARC_WIDTH, ARC_HEIGHT);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_arc_set_value(arc, ARC_DEFAULT);
  lv_obj_center(arc);
  lv_obj_align(arc, LV_ALIGN_CENTER, ARC_X, ARC_Y);
  lv_obj_add_event_cb(arc, arc_event_handler, LV_EVENT_ALL, NULL);

  label = lv_label_create(obj);
  lv_label_set_text_fmt(label, "%d*C", meterValue);
  lv_obj_align_to(label, arc, LV_ALIGN_CENTER, ARC_X - 50, ARC_Y - 10);

}

/**********************************************************************
* Main Program Loop
***********************************************************************/

void loop() { 
  /* Feed LVGL engine */
  lv_timer_handler();
  delay(100);
}