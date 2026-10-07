/**
 * @file icu_telemetry.c
 * @brief Telemetry serialization implementation.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#include "telemetry/icu_telemetry.h"
#include "core/icu_fsm.h"
#include <stdio.h>

int icu_telemetry_format_json(const icu_telemetry_packet_t *packet, char *buffer, size_t max_len) {
    if (packet == NULL || buffer == NULL || max_len == 0) {
        return 0;
    }

    return snprintf(buffer, max_len,
        "{"
        "\"seq\":%lu,"
        "\"uptime_s\":%lu,"
        "\"state\":\"%s\","
        "\"alarm\":\"%s\","
        "\"msg\":\"%s\","
        "\"vitals\":{"
            "\"incline_pitch_deg\":%.1f,"
            "\"bed_roll_deg\":%.1f,"
            "\"total_accel_g\":%.2f,"
            "\"patient_dist_cm\":%.1f,"
            "\"patient_present\":%s,"
            "\"ambient_adc\":%u,"
            "\"night_mode\":%s,"
            "\"ir_left\":%s,"
            "\"ir_right\":%s,"
            "\"nurse_call\":%s"
        "}"
        "}",
        (unsigned long)packet->sequence_id,
        (unsigned long)packet->uptime_seconds,
        icu_fsm_state_to_string(packet->current_state),
        icu_fsm_alarm_level_to_string(packet->alarm_level),
        packet->status_message,
        packet->sensors.pitch_angle_deg,
        packet->sensors.roll_angle_deg,
        packet->sensors.total_accel_g,
        packet->sensors.patient_distance_cm,
        packet->sensors.is_patient_in_bed ? "true" : "false",
        packet->sensors.ambient_lux_raw,
        packet->sensors.is_night_mode ? "true" : "false",
        packet->sensors.ir_left_edge_tripped ? "true" : "false",
        packet->sensors.ir_right_edge_tripped ? "true" : "false",
        packet->sensors.nurse_call_switch_pressed ? "true" : "false"
    );
}

int icu_telemetry_format_console(const icu_telemetry_packet_t *packet, char *buffer, size_t max_len) {
    if (packet == NULL || buffer == NULL || max_len == 0) {
        return 0;
    }

    return snprintf(buffer, max_len,
        "[%04lus] STATE: %-22s | ALARM: %-15s | INCLINE: %4.1f deg | DIST: %4.1f cm | SHOCK: %4.2f g\r\n"
        "       STATUS: %s\r\n",
        (unsigned long)packet->uptime_seconds,
        icu_fsm_state_to_string(packet->current_state),
        icu_fsm_alarm_level_to_string(packet->alarm_level),
        packet->sensors.pitch_angle_deg,
        packet->sensors.patient_distance_cm,
        packet->sensors.total_accel_g,
        packet->status_message
    );
}
