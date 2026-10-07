/**
 * @file icu_fsm.h
 * @brief Clinical Bedside Finite-State Machine (FSM) Coordinator.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#ifndef ICU_FSM_H
#define ICU_FSM_H

#include "icu_types.h"
#include "core/patient_monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Top-level ICU FSM state container.
 */
typedef struct {
    icu_state_t           current_state;
    icu_state_t           previous_state;
    icu_alarm_level_t     active_alarm_level;
    buzzer_pattern_t      buzzer_pattern;
    patient_monitor_ctx_t monitor_ctx;
    uint32_t              state_dwell_time_ticks;
} icu_fsm_t;

/**
 * @brief Initializes the FSM instance to BED_VACANT state.
 *
 * @param[out] fsm Pointer to FSM struct.
 */
void icu_fsm_init(icu_fsm_t *fsm);

/**
 * @brief Executes one tick of the FSM with new sensor inputs.
 *
 * @param[in,out] fsm     Pointer to FSM instance.
 * @param[in]     sensors Current sensor frame.
 * @param[out]    packet  Populated telemetry packet.
 */
void icu_fsm_step(icu_fsm_t *fsm,
                  const icu_sensor_data_t *sensors,
                  icu_telemetry_packet_t *packet);

/**
 * @brief Returns human-readable state string.
 *
 * @param[in] state FSM state enum.
 * @return const char* String description.
 */
const char* icu_fsm_state_to_string(icu_state_t state);

/**
 * @brief Returns human-readable alarm level string.
 *
 * @param[in] level Alarm priority level.
 * @return const char* String description.
 */
const char* icu_fsm_alarm_level_to_string(icu_alarm_level_t level);

#ifdef __cplusplus
}
#endif

#endif /* ICU_FSM_H */
