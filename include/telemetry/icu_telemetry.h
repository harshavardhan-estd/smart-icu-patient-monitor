/**
 * @file icu_telemetry.h
 * @brief Telemetry formatting for IoT Central Nurse Station and UART debug streaming.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#ifndef ICU_TELEMETRY_H
#define ICU_TELEMETRY_H

#include <stddef.h>
#include "icu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Formats telemetry packet into JSON string for IoT gateway / ThingsBoard / MQTT.
 *
 * @param[in]  packet  Source telemetry packet.
 * @param[out] buffer  Destination char buffer.
 * @param[in]  max_len Maximum buffer length.
 * @return int Number of characters written.
 */
int icu_telemetry_format_json(const icu_telemetry_packet_t *packet, char *buffer, size_t max_len);

/**
 * @brief Formats telemetry into an ASCII bedside monitoring dashboard for Serial terminal.
 *
 * @param[in]  packet  Source telemetry packet.
 * @param[out] buffer  Destination char buffer.
 * @param[in]  max_len Maximum buffer length.
 * @return int Number of characters written.
 */
int icu_telemetry_format_console(const icu_telemetry_packet_t *packet, char *buffer, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* ICU_TELEMETRY_H */
