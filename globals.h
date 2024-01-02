#pragma once

#define WHITE 0xffffff
#define BLACK 0x000000
#define ARDUINO 0x03989e
#define GRID 0x046f87

#define BRIGHTNESS 30       // start up screen brightness
#define MIN_BRIGHTNESS 10   // minimum brightness
#define MAX_BRIGHTNESS 90   // maximum brightness

// SLIDER
#define SLIDER_X 0
#define SLIDER_Y 70
#define SLIDER_LABEL "Brightness"

// ROLLERS
#define DAYS_X 250
#define DAYS_Y -110
#define DAYS_INDEX 1
#define DAYS_ROWS 4
#define DAYS_WIDTH 110
#define DAYS_LABEL "Days"

#define TEMP_X 80
#define TEMP_Y 110
#define TEMP_INDEX 1
#define TEMP_ROWS 3
#define TEMP_WIDTH 50
#define TEMP_LABEL "Scale"

#define BRIGHT_X 60
#define BRIGHT_Y 85
#define BRIGHT_INDEX 1
#define BRIGHT_ROWS 3
#define BRIGHT_WIDTH 100
#define BRIGHT_LABEL "Temp"

// METER
#define METER_WIDTH 135
#define METER_HEIGHT 135
#define METER_X -120
#define METER_Y 90
#define METER_MINOR_W 1
#define METER_MINOR_L 10
#define METER_MAJOR_W 2
#define METER_MAJOR_L 15
#define METER_NEEDLE_W 3
#define METER_MIN 0
#define METER_MAX 40
#define METER_COLD 10
#define METER_WARM 30
#define METER_DEFAULT 25
#define METER_LABEL "Temp *C"
int meterValue = 25;

//ARC
#define ARC_X 50
#define ARC_Y 10
#define ARC_WIDTH 150
#define ARC_HEIGHT 150
#define ARC_MIN 0
#define ARC_MAX 100
#define ARC_DEFAULT 25
#define ARC_LABEL "25*C"

