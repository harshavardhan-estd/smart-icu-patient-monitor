/**
 * @file hal_sensors.c
 * @brief Hardware Abstraction Layer implementation with deterministic sensor emulation.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#include "hal/hal_sensors.h"
#include "icu_config.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

static icu_mock_scenario_t s_current_scenario = SCENARIO_NORMAL_RESTING;
static bool                s_nurse_call_latched = false;
static buzzer_pattern_t    s_active_buzzer = BUZZER_PATTERN_OFF;
static icu_alarm_level_t   s_active_leds = ALARM_LEVEL_NONE;
static uint32_t            s_tick_counter = 0;

icu_status_t hal_sensors_init(void) {
    s_current_scenario = SCENARIO_NORMAL_RESTING;
    s_nurse_call_latched = false;
    s_active_buzzer = BUZZER_PATTERN_OFF;
    s_active_leds = ALARM_LEVEL_NONE;
    s_tick_counter = 0;
    return ICU_OK;
}

void hal_sensors_set_mock_scenario(icu_mock_scenario_t scenario) {
    s_current_scenario = scenario;
    if (scenario == SCENARIO_NURSE_CALL_TRIGGER) {
        s_nurse_call_latched = true;
    }
}

void hal_emergency_clear_nurse_call(void) {
    s_nurse_call_latched = false;
}

void hal_actuators_set_buzzer(buzzer_pattern_t pattern) {
    s_active_buzzer = pattern;
}

void hal_actuators_set_leds(icu_alarm_level_t level) {
    s_active_leds = level;
}

icu_status_t hal_sensors_read_all(icu_sensor_data_t *data) {
    if (data == NULL) {
        return ICU_ERR_NULL_PTR;
    }

    memset(data, 0, sizeof(icu_sensor_data_t));
    s_tick_counter++;

    /* Base measurements */
    float ax = 0.05f;
    float ay = 0.35f;
    float az = 0.93f;
    float distance_cm = 32.0f; /* Normal patient in bed distance */
    uint16_t ldr_raw = 1450U;  /* Normal daylight */
    bool ir_left = false;
    bool ir_right = false;

    switch (s_current_scenario) {
        case SCENARIO_NORMAL_RESTING:
            /* Safe Fowler's angle ~22 degrees, patient resting */
            ax = 0.02f;
            ay = 0.37f;
            az = 0.92f;
            distance_cm = 30.0f;
            ldr_raw = 1400U;
            break;

        case SCENARIO_BED_EXIT_ATTEMPT:
            /* Patient shifting weight towards left rail */
            ax = 0.15f;
            ay = 0.55f;
            az = 0.82f;
            distance_cm = 48.0f;
            ir_left = true; /* Left IR boundary broken */
            break;

        case SCENARIO_PATIENT_FALL:
            /* Severe acceleration shock (> 2.8g) followed by mattress vacancy */
            if ((s_tick_counter % 8) < 3) {
                ax = 1.85f;
                ay = 2.10f;
                az = 0.65f; /* Total |A| > 2.8g */
            } else {
                ax = 0.00f;
                ay = 0.05f;
                az = 0.99f;
            }
            distance_cm = 115.0f; /* Empty mattress (patient fallen onto floor) */
            ir_right = true;
            break;

        case SCENARIO_NURSE_CALL_TRIGGER:
            /* Patient pressed pendant */
            s_nurse_call_latched = true;
            distance_cm = 28.0f;
            break;

        case SCENARIO_ABNORMAL_TILT:
            /* Steep Fowler incline > 65 degrees */
            ax = 0.10f;
            ay = 0.92f;
            az = 0.38f; /* Incline > 67 deg */
            distance_cm = 35.0f;
            break;

        case SCENARIO_EMPTY_BED:
            /* No patient in bed */
            distance_cm = 120.0f;
            break;

        default:
            break;
    }

    /* Compute Vector Magnitude and Angles */
    data->accel_x_g = ax;
    data->accel_y_g = ay;
    data->accel_z_g = az;
    data->total_accel_g = sqrtf(ax * ax + ay * ay + az * az);

    /* Clinical Fowler / Bed incline: Pitch = atan2(ay, az) */
    data->pitch_angle_deg = atan2f(ay, az) * (180.0f / (float)M_PI);
    data->roll_angle_deg  = atan2f(ax, az) * (180.0f / (float)M_PI);

    /* Ultrasonic Occupancy Evaluation */
    data->patient_distance_cm = distance_cm;
    data->is_patient_in_bed = (distance_cm >= ICU_PATIENT_MIN_DIST_CM &&
                               distance_cm <= ICU_PATIENT_MAX_DIST_CM);

    /* Ambient Lighting */
    data->ambient_lux_raw = ldr_raw;
    data->is_night_mode = (ldr_raw < ICU_LDR_NIGHT_THRESHOLD_ADC);

    /* Boundary Infrared */
    data->ir_left_edge_tripped = ir_left;
    data->ir_right_edge_tripped = ir_right;

    /* Emergency controls */
    data->nurse_call_switch_pressed = s_nurse_call_latched;

    return ICU_OK;
}
