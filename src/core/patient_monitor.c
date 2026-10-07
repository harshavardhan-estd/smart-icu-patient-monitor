/**
 * @file patient_monitor.c
 * @brief Clinical patient safety algorithms: fall detection, bed occupancy, and incline analysis.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#include "core/patient_monitor.h"
#include <string.h>

void patient_monitor_init(patient_monitor_ctx_t *ctx) {
    if (ctx == NULL) return;
    memset(ctx, 0, sizeof(patient_monitor_ctx_t));
}

void patient_monitor_evaluate(patient_monitor_ctx_t *ctx,
                              const icu_sensor_data_t *sensors,
                              icu_state_t *state,
                              icu_alarm_level_t *alarm) {
    if (ctx == NULL || sensors == NULL || state == NULL || alarm == NULL) {
        return;
    }

    /* 1. Track Peak Impact Acceleration */
    if (sensors->total_accel_g > ctx->recent_peak_accel_g) {
        ctx->recent_peak_accel_g = sensors->total_accel_g;
    }

    /* Check for sudden fall shock impact */
    if (sensors->total_accel_g >= ICU_FALL_IMPACT_THRESHOLD_G) {
        ctx->fall_shock_detected = true;
        ctx->fall_shock_latch_ticks = 20; /* Keep active for 20 ticks (~1.0s) */
    } else if (ctx->fall_shock_latch_ticks > 0) {
        ctx->fall_shock_latch_ticks--;
    } else {
        ctx->fall_shock_detected = false;
        ctx->recent_peak_accel_g = sensors->total_accel_g;
    }

    /* 2. Debounce Bed Vacancy */
    if (!sensors->is_patient_in_bed) {
        ctx->consecutive_vacant_ticks++;
    } else {
        ctx->consecutive_vacant_ticks = 0;
    }

    bool is_bed_confirmed_vacant = (ctx->consecutive_vacant_ticks >= ICU_BED_VACANT_DEBOUNCE_CYCLES);

    /* -------------------------------------------------------------
     * CLINICAL ALARM ARBITRATION (Strict IEC 60601-1-8 Priority)
     * ------------------------------------------------------------- */

    /* Priority 1A: Nurse Call Switch Pressed (Emergency Button) */
    if (sensors->nurse_call_switch_pressed) {
        *state = ICU_STATE_NURSE_CALL;
        *alarm = ALARM_LEVEL_CRITICAL;
        return;
    }

    /* Priority 1B: Fall Detected (Impact shock + bed vacancy/rail breach) */
    if (ctx->fall_shock_detected && (is_bed_confirmed_vacant || sensors->ir_left_edge_tripped || sensors->ir_right_edge_tripped)) {
        *state = ICU_STATE_PATIENT_FALL;
        *alarm = ALARM_LEVEL_CRITICAL;
        return;
    }

    /* Priority 2A: Bed Exit Attempt (IR Boundary Rail Breached) */
    if (sensors->ir_left_edge_tripped || sensors->ir_right_edge_tripped) {
        *state = ICU_STATE_BED_EXIT_ATTEMPT;
        *alarm = ALARM_LEVEL_WARNING;
        return;
    }

    /* Priority 2B: Abnormal Bed Tilt / Incline Exceeds Safe Fowler Range */
    if (sensors->pitch_angle_deg > ICU_BED_TILT_SAFE_MAX_DEG ||
        sensors->pitch_angle_deg < ICU_BED_TILT_SAFE_MIN_DEG) {
        *state = ICU_STATE_ABNORMAL_BED_TILT;
        *alarm = ALARM_LEVEL_WARNING;
        return;
    }

    /* Priority 3: Confirmed Bed Vacant (Patient out of bed without fall) */
    if (is_bed_confirmed_vacant) {
        *state = ICU_STATE_BED_VACANT;
        *alarm = ALARM_LEVEL_INFO;
        return;
    }

    /* Priority 4: Normal Stable Patient Bedside Monitoring */
    *state = ICU_STATE_NORMAL_MONITORING;
    *alarm = ALARM_LEVEL_NONE;
}
