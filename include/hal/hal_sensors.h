/**
 * @file hal_sensors.h
 * @brief Hardware Abstraction Layer for ICU sensors, OLED, buzzer, and controls.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#ifndef HAL_SENSORS_H
#define HAL_SENSORS_H

#include <stdbool.h>
#include <stdint.h>
#include "icu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Test and simulation scenarios for automated verification.
 */
typedef enum {
    SCENARIO_NORMAL_RESTING     = 0,  ///< Patient resting peacefully; nominal tilt and distance
    SCENARIO_BED_EXIT_ATTEMPT   = 1,  ///< Patient moving to left edge (IR left broken)
    SCENARIO_PATIENT_FALL       = 2,  ///< Rapid high-g shock impact followed by bed vacancy
    SCENARIO_NURSE_CALL_TRIGGER = 3,  ///< Emergency call button pressed
    SCENARIO_ABNORMAL_TILT      = 4,  ///< Head of bed elevated dangerously high (> 65°)
    SCENARIO_EMPTY_BED          = 5   ///< Mattress completely unoccupied (> 120cm)
} icu_mock_scenario_t;

/**
 * @brief Initializes all peripheral drivers (I2C, ADC, GPIO, TIM).
 *
 * @return icu_status_t ICU_OK on success.
 */
icu_status_t hal_sensors_init(void);

/**
 * @brief Reads a full synchronized sensor frame (IMU, Ultrasonic, LDR, IR, Switch).
 *
 * @param[out] data Destination sensor data struct.
 * @return icu_status_t ICU_OK on success.
 */
icu_status_t hal_sensors_read_all(icu_sensor_data_t *data);

/**
 * @brief Updates the acoustic alarm output pattern.
 *
 * @param[in] pattern Desired buzzer cadence (Off, Tick, Intermittent, Urgent, Continuous).
 */
void hal_actuators_set_buzzer(buzzer_pattern_t pattern);

/**
 * @brief Updates the tri-color clinical status LEDs.
 *
 * @param[in] level Current alarm urgency level.
 */
void hal_actuators_set_leds(icu_alarm_level_t level);

/**
 * @brief Clears latched nurse-call or emergency events.
 */
void hal_emergency_clear_nurse_call(void);

/**
 * @brief Configures synthetic test scenario for deterministic unit testing and host simulation.
 *
 * @param[in] scenario Test scenario to simulate.
 */
void hal_sensors_set_mock_scenario(icu_mock_scenario_t scenario);

#ifdef __cplusplus
}
#endif

#endif /* HAL_SENSORS_H */
