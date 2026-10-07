/**
 * @file test_patient_monitor.c
 * @brief Unit tests for patient posture algorithms, fall thresholds, and occupancy debouncing.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#include "unity/unity.h"
#include "core/patient_monitor.h"
#include <stdio.h>

static int s_tests_run = 0;
static int s_tests_passed = 0;

void test_incline_angle_triggers_abnormal_tilt(void) {
    s_tests_run++;
    patient_monitor_ctx_t ctx;
    patient_monitor_init(&ctx);

    icu_sensor_data_t sensors = {
        .is_patient_in_bed = true,
        .pitch_angle_deg = 68.0f /* Steep tilt > 60 degrees */
    };

    icu_state_t state;
    icu_alarm_level_t alarm;
    patient_monitor_evaluate(&ctx, &sensors, &state, &alarm);

    TEST_ASSERT_EQUAL_INT(ICU_STATE_ABNORMAL_BED_TILT, state);
    TEST_ASSERT_EQUAL_INT(ALARM_LEVEL_WARNING, alarm);

    s_tests_passed++;
    printf("PASS: test_incline_angle_triggers_abnormal_tilt (Incline: 68 deg -> Warning)\n");
}

void test_bed_exit_attempt_triggered_by_ir_rails(void) {
    s_tests_run++;
    patient_monitor_ctx_t ctx;
    patient_monitor_init(&ctx);

    icu_sensor_data_t sensors = {
        .is_patient_in_bed = true,
        .pitch_angle_deg = 25.0f,
        .ir_left_edge_tripped = true /* Left bed rail barrier broken */
    };

    icu_state_t state;
    icu_alarm_level_t alarm;
    patient_monitor_evaluate(&ctx, &sensors, &state, &alarm);

    TEST_ASSERT_EQUAL_INT(ICU_STATE_BED_EXIT_ATTEMPT, state);
    TEST_ASSERT_EQUAL_INT(ALARM_LEVEL_WARNING, alarm);

    s_tests_passed++;
    printf("PASS: test_bed_exit_attempt_triggered_by_ir_rails\n");
}

int main(void) {
    printf("--- Running ICU Patient Monitor Algorithm Tests ---\n");
    test_incline_angle_triggers_abnormal_tilt();
    test_bed_exit_attempt_triggered_by_ir_rails();
    printf("Results: %d/%d passed.\n\n", s_tests_passed, s_tests_run);
    return (s_tests_passed == s_tests_run) ? 0 : 1;
}
