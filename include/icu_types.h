/**
 * @file icu_types.h
 * @brief Common types, clinical status enumerations, and telemetry structures.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#ifndef ICU_TYPES_H
#define ICU_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief System return codes.
 */
typedef enum {
    ICU_OK                   =  0,
    ICU_ERR_GENERIC          = -1,
    ICU_ERR_NULL_PTR         = -2,
    ICU_ERR_INVALID_PARAM    = -3,
    ICU_ERR_SENSOR_TIMEOUT   = -4,
    ICU_ERR_BUS_FAILURE      = -5
} icu_status_t;

/**
 * @brief Clinical Bed & Patient States (Finite-State Machine).
 */
typedef enum {
    ICU_STATE_BED_VACANT        = 0,  ///< No patient detected in bed
    ICU_STATE_NORMAL_MONITORING = 1,  ///< Patient resting safely within bed limits
    ICU_STATE_BED_EXIT_ATTEMPT  = 2,  ///< Patient near edge (IR boundary triggered)
    ICU_STATE_PATIENT_FALL      = 3,  ///< High impact shock + sudden bed exit (EMERGENCY)
    ICU_STATE_NURSE_CALL        = 4,  ///< Patient pressed emergency pendant button
    ICU_STATE_ABNORMAL_BED_TILT = 5   ///< Bed angle exceeds clinical safe threshold
} icu_state_t;

/**
 * @brief Clinical Alarm Urgency Level (IEC 60601-1-8 standard alignment).
 */
typedef enum {
    ALARM_LEVEL_NONE     = 0,  ///< Normal operations; green indicators
    ALARM_LEVEL_INFO     = 1,  ///< Informational status (e.g. ambient dimming)
    ALARM_LEVEL_WARNING  = 2,  ///< Medium priority (bed exit attempt, tilt advisory)
    ALARM_LEVEL_CRITICAL = 3   ///< High priority (fall detected, nurse call active)
} icu_alarm_level_t;

/**
 * @brief Buzzer acoustic pattern cadence.
 */
typedef enum {
    BUZZER_PATTERN_OFF        = 0,  ///< Silent
    BUZZER_PATTERN_TICK       = 1,  ///< Subtle 50ms chirp (user feedback)
    BUZZER_PATTERN_INTERMITTENT = 2,///< 500ms ON / 500ms OFF (Warning / Bed Exit)
    BUZZER_PATTERN_URGENT_PULSE = 3,///< 200ms ON / 100ms OFF (Nurse Call)
    BUZZER_PATTERN_CONTINUOUS   = 4 ///< Constant siren (Patient Fall Emergency)
} buzzer_pattern_t;

/**
 * @brief Raw & Filtered Sensor Measurements Vector.
 */
typedef struct {
    /* 6-Axis IMU (MPU6050) */
    float accel_x_g;           ///< X-axis acceleration in g
    float accel_y_g;           ///< Y-axis acceleration in g
    float accel_z_g;           ///< Z-axis acceleration in g
    float total_accel_g;       ///< Total acceleration magnitude vector |A|
    float pitch_angle_deg;     ///< Bed Fowler angle / head elevation in degrees
    float roll_angle_deg;      ///< Lateral bed tilt in degrees

    /* Proximity & Distance (HC-SR04) */
    float patient_distance_cm; ///< Distance to patient surface in centimeters
    bool  is_patient_in_bed;   ///< Filtered occupancy boolean

    /* Ambient Lighting (LDR) */
    uint16_t ambient_lux_raw;  ///< Raw 12-bit ADC reading (0 - 4095)
    bool     is_night_mode;    ///< True if ambient lighting is low

    /* Boundary Infrared Sensors (IR Barrier 1 & 2) */
    bool ir_left_edge_tripped;  ///< Left bed-rail boundary break
    bool ir_right_edge_tripped; ///< Right bed-rail boundary break

    /* Emergency Controls */
    bool nurse_call_switch_pressed; ///< Emergency push button latch state
} icu_sensor_data_t;

/**
 * @brief Structured Medical Bedside Telemetry Packet.
 */
typedef struct {
    uint32_t           sequence_id;      ///< Rolling telemetry sequence number
    uint32_t           uptime_seconds;   ///< System uptime in seconds
    icu_state_t        current_state;    ///< FSM active state
    icu_alarm_level_t  alarm_level;      ///< Active clinical alarm priority
    buzzer_pattern_t   buzzer_status;    ///< Acoustic alert pattern
    icu_sensor_data_t  sensors;          ///< Complete sensor measurement snapshot
    const char*        status_message;   ///< Human-readable status descriptor
} icu_telemetry_packet_t;

#ifdef __cplusplus
}
#endif

#endif /* ICU_TYPES_H */
