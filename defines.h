#ifndef DEFINES_H
#define DEFINES_H

#define APPLICATION_NAME "Origin II Sensor Array Test"
#define APPLICATION_VERSION "1.0.0"
#define APPLICATION_ORGANIZATION "LocusRobotics"

#define TAG_WINDOW_GEOMETRY "geometry"
#define TAG_WINDOW_STATE "windowstate"
#define LOG_MAX_LEN 65535
#define LOG_TRIM_LENGTH 2048

#define SERIAL_REG_EXPR "^([0-9]{2})([0-9]{2})([0-1]{1}[0-9]{1})([a-z|A-Z]{1})([0-9]{2})(x|X)([0-9]{5})"

#define ECON_MONO_DESCRIPTION "e-con's 1MP Monochrome Camera"
#define BASLER_COLOR_DESCRIPTION "daA1280-54uc"
#define BASLER_MONO_DESCRIPTION "daA1280-54um"
#define REALSENSE_DESCRIPTION "Intel RealSense D435"

#define LABEL_TEXT_PASS "PASS"
#define LABEL_TEXT_IDLE "IDLE"
#define LABEL_TEXT_INCOMPLETE "INCP"
#define LABEL_TEXT_TESTING "TESTING"
#define LABEL_TEXT_FAIL "FAIL"

#define STYLESHEET_BANNER_BLUE "QLabel { background-color : blue; color : white; }"
#define STYLESHEET_BANNER_GREEN "QLabel { background-color : green; color : black; }"
#define STYLESHEET_BANNER_YELLOW "QLabel { background-color : yellow; color : black; }"
#define STYLESHEET_BANNER_RED "QLabel { background-color : red; color : black; }"
#define STYLESHEET_BANNER_DEFAULT ""

#define LOG_TIMESTAMP_FORMAT "yyyyMMdd_hh-mm-ss-zzz"
#define LOG_FILE_NAME "%1_%2_SENSOR_TEST.txt"
#define LOG_FILE_PATH "C:\\TestData\\SENSOR_ARRY_TEST\\"
#define LOG_SEPARATOR '='
#define LOG_SEPARATOR_LEN 85

#define TEST_NAME_TOTAL_DEVICES "DEV_TOTAL"
#define TOTAL_DEVICES_LOW_LIM 5
#define TOTAL_DEVICES_HIGH_LIM 5

#define TEST_NAME_REALSENSE_COUNT "RS_DEVICE_COUNT"
#define REALSENSE_COUNT_LOW_LIM 2
#define REALSENSE_COUNT_HIGH_LIM 2

#define TEST_NAME_CAMERA_COUNT "CAM_DEV_COUNT"
#define CAMERA_COUNT_LOW_LIM 3
#define CAMERA_COUNT_HIGH_LIM 3

#define TEST_NAME_COLOR_CAM_COUNT "CAM_COLOR_COUNT"
#define COLOR_CAM_COUNT_LOW_LIM 1
#define COLOR_CAM_COUNT_HIGH_LIM 1

#define TEST_NAME_MONO_CAM_COUNT "CAM_MONO_COUNT"
#define MONO_CAM_COUNT_LOW_LIM 2
#define MONO_CAM_COUNT_HIGH_LIM 2

#define TEST_SENSOR_CONFIGURATION "ARRAY_CONFIG"
#define ARRAY_CONFIG_LOW_LIM 1
#define ARRAY_CONFIG_HIGH_LIM 1
#endif // DEFINES_H
