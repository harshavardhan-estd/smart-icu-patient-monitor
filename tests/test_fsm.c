/**
 * @file test_fsm.c
 * @brief Unit tests for Bedside Finite-State Machine and alert arbitration.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#include "unity/unity.h"
#include "core/icu_fsm.h"
#include "hal/hal_sensors.h"
#include <stdio.h>

static int s_tests_run = 0;
static int s_tests_passed = 0;

void test_fsm_initial_state_vacant(void) {
    s_tests_run++;
    icu_fsm_t fsm;
    icu_fsm_init(&fsm);

    TEST_ASSERT_EQUAL_INT(ICU_STATE_BED_VACANT, fsm.current_state);
    TEST_ASSERT_EQUAL_INT(BUZZER_PATTERN_OFF, fsm.buzzer_pattern);

    s_tests_passed++;
    printf("PASS: test_fsm_initial_state_vacant\n");
}

void test_fsm_nurse_call_overrides_other_alerts(void) {
    s_tests_run++;
    icu_fsm_t fsm;
    icu_fsm_init(&fsm);

    icu_sensor_data_t sensors = {
        .is_patient_in_bed = true,
        .pitch_angle_deg = 70.0f,               /* Abnormal tilt */
        .ir_left_edge_tripped = true,           /* Bed exit */
        .nurse_call_switch_pressed = true       /* Nurse call pressed */
    };

    icu_telemetry_packet_t pkt;
    icu_fsm_step(&fsm, &sensors, &pkt);

    /* Nurse call MUST take absolute highest priority */
    TEST_ASSERT_EQUAL_INT(ICU_STATE_NURSE_CALL, fsm.current_state);
    TEST_ASSERT_EQUAL_INT(ALARM_LEVEL_CRITICAL, fsm.active_alarm_level);
    TEST_ASSERT_EQUAL_INT(BUZZER_PATTERN_URGENT_PULSE, fsm.buzzer_pattern);

    s_tests_passed++;
    printf("PASS: test_fsm_nurse_call_overrides_other_alerts\n");
}

void test_fsm_fall_detection_triggers_continuous_siren(void) {
    s_tests_run++;
    icu_fsm_t fsm;
    icu_fsm_init(&fsm);

    icu_sensor_data_t sensors = {
        .total_accel_g = 3.2f,        /* Shock > 2.6g */
        .is_patient_in_bed = false,   /* Empty bed */
        .patient_distance_cm = 120.0f
    };

    icu_telemetry_packet_t pkt;
    /* Run 5 ticks to confirm vacancy + shock */
    for (int i = 0; i < 6; i++) {
        icu_fsm_step(&fsm, &sensors, &pkt);
    }

    TEST_ASSERT_EQUAL_INT(ICU_STATE_PATIENT_FALL, fsm.current_state);
    TEST_ASSERT_EQUAL_INT(ALARM_LEVEL_CRITICAL, fsm.active_alarm_level);
    TEST_ASSERT_EQUAL_INT(BUZZER_PATTERN_CONTINUOUS, fsm.buzzer_pattern);

    s_tests_passed++;
    printf("PASS: test_fsm_fall_detection_triggers_continuous_siren\n");
}

int main(void) {
    printf("--- Running ICU FSM & Alarm Arbitration Tests ---\n");
    test_fsm_initial_state_vacant();
    test_fsm_nurse_call_overrides_other_alerts();
    test_fsm_fall_detection_triggers_continuous_siren();
    printf("Results: %d/%d passed.\n\n", s_tests_passed, s_tests_run);
    return (s_tests_passed == s_tests_run) ? 0 : 1;
}
