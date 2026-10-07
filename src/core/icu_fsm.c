/**
 * @file icu_fsm.c
 * @brief Clinical Bedside Finite-State Machine implementation.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#include "core/icu_fsm.h"
#include "hal/hal_sensors.h"
#include <string.h>

void icu_fsm_init(icu_fsm_t *fsm) {
    if (fsm == NULL) return;
    memset(fsm, 0, sizeof(icu_fsm_t));
    fsm->current_state = ICU_STATE_BED_VACANT;
    fsm->previous_state = ICU_STATE_BED_VACANT;
    fsm->active_alarm_level = ALARM_LEVEL_INFO;
    fsm->buzzer_pattern = BUZZER_PATTERN_OFF;
    patient_monitor_init(&fsm->monitor_ctx);
}

void icu_fsm_step(icu_fsm_t *fsm,
                  const icu_sensor_data_t *sensors,
                  icu_telemetry_packet_t *packet) {
    if (fsm == NULL || sensors == NULL || packet == NULL) {
        return;
    }

    /* 1. Evaluate Sensor Inputs through Clinical Logic */
    icu_state_t next_state = fsm->current_state;
    icu_alarm_level_t next_alarm = ALARM_LEVEL_NONE;

    patient_monitor_evaluate(&fsm->monitor_ctx, sensors, &next_state, &next_alarm);

    /* Track State Transitions */
    if (next_state != fsm->current_state) {
        fsm->previous_state = fsm->current_state;
        fsm->current_state = next_state;
        fsm->state_dwell_time_ticks = 0;
    } else {
        fsm->state_dwell_time_ticks++;
    }

    fsm->active_alarm_level = next_alarm;

    /* 2. Configure Actuators based on State */
    buzzer_pattern_t buzzer_patt = BUZZER_PATTERN_OFF;
    const char *status_msg = "NORMAL";

    switch (fsm->current_state) {
        case ICU_STATE_BED_VACANT:
            buzzer_patt = BUZZER_PATTERN_OFF;
            status_msg = "BED VACANT - Awaiting Admission";
            break;

        case ICU_STATE_NORMAL_MONITORING:
            buzzer_patt = BUZZER_PATTERN_OFF;
            status_msg = "PATIENT STABLE - Monitoring";
            break;

        case ICU_STATE_BED_EXIT_ATTEMPT:
            buzzer_patt = BUZZER_PATTERN_INTERMITTENT;
            status_msg = "WARNING: Bed Exit Attempt Detected!";
            break;

        case ICU_STATE_PATIENT_FALL:
            buzzer_patt = BUZZER_PATTERN_CONTINUOUS;
            status_msg = "CRITICAL: Patient Fall Emergency!";
            break;

        case ICU_STATE_NURSE_CALL:
            buzzer_patt = BUZZER_PATTERN_URGENT_PULSE;
            status_msg = "EMERGENCY: Nurse Call Button Pressed!";
            break;

        case ICU_STATE_ABNORMAL_BED_TILT:
            buzzer_patt = BUZZER_PATTERN_INTERMITTENT;
            status_msg = "ADVISORY: Bed Incline Angle Exceeded Safe Limit";
            break;
    }

    fsm->buzzer_pattern = buzzer_patt;

    /* Update Physical Actuators */
    hal_actuators_set_buzzer(buzzer_patt);
    hal_actuators_set_leds(fsm->active_alarm_level);

    /* 3. Populate Output Telemetry Packet */
    static uint32_t s_seq = 0;
    packet->sequence_id = s_seq++;
    packet->uptime_seconds = s_seq / 20; /* 20 ticks per second (50ms tick) */
    packet->current_state = fsm->current_state;
    packet->alarm_level = fsm->active_alarm_level;
    packet->buzzer_status = fsm->buzzer_pattern;
    packet->sensors = *sensors;
    packet->status_message = status_msg;
}

const char* icu_fsm_state_to_string(icu_state_t state) {
    switch (state) {
        case ICU_STATE_BED_VACANT:        return "BED_VACANT";
        case ICU_STATE_NORMAL_MONITORING: return "NORMAL_MONITORING";
        case ICU_STATE_BED_EXIT_ATTEMPT:  return "BED_EXIT_ATTEMPT";
        case ICU_STATE_PATIENT_FALL:      return "PATIENT_FALL_EMERGENCY";
        case ICU_STATE_NURSE_CALL:        return "NURSE_CALL_ACTIVE";
        case ICU_STATE_ABNORMAL_BED_TILT: return "ABNORMAL_BED_TILT";
        default:                          return "UNKNOWN_STATE";
    }
}

const char* icu_fsm_alarm_level_to_string(icu_alarm_level_t level) {
    switch (level) {
        case ALARM_LEVEL_NONE:     return "NONE (Safe)";
        case ALARM_LEVEL_INFO:     return "INFO";
        case ALARM_LEVEL_WARNING:  return "MEDIUM WARNING";
        case ALARM_LEVEL_CRITICAL: return "HIGH CRITICAL ALARM";
        default:                   return "UNKNOWN";
    }
}
