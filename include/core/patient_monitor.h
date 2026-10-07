/**
 * @file patient_monitor.h
 * @brief Clinical patient safety algorithms: fall detection, bed occupancy, and incline analysis.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#ifndef PATIENT_MONITOR_H
#define PATIENT_MONITOR_H

#include <stdbool.h>
#include <stdint.h>
#include "icu_types.h"
#include "icu_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Algorithmic state tracker for fall and occupancy analysis.
 */
typedef struct {
    uint32_t consecutive_vacant_ticks;
    uint32_t fall_shock_latch_ticks;
    bool     fall_shock_detected;
    float    recent_peak_accel_g;
} patient_monitor_ctx_t;

/**
 * @brief Initializes the patient monitor algorithmic context.
 *
 * @param[out] ctx Pointer to context struct.
 */
void patient_monitor_init(patient_monitor_ctx_t *ctx);

/**
 * @brief Processes latest sensor telemetry and determines active patient state.
 *
 * @param[in,out] ctx     Monitor context holding history.
 * @param[in]     sensors Current sensor frame.
 * @param[out]    state   Evaluated clinical state.
 * @param[out]    alarm   Evaluated alarm priority level.
 */
void patient_monitor_evaluate(patient_monitor_ctx_t *ctx,
                              const icu_sensor_data_t *sensors,
                              icu_state_t *state,
                              icu_alarm_level_t *alarm);

#ifdef __cplusplus
}
#endif

#endif /* PATIENT_MONITOR_H */
