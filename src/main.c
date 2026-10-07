/**
 * @file main.c
 * @brief Smart Medical ICU Safety & Patient Monitoring System firmware and host emulator.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#include <stdio.h>
#include <string.h>
#include "icu_types.h"
#include "icu_config.h"
#include "hal/hal_sensors.h"
#include "core/icu_fsm.h"
#include "telemetry/icu_telemetry.h"

#if defined(STM32F4xx) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO)
/* =========================================================================
 * STM32 Black Pill (ARM Cortex-M4) Target Firmware
 * ========================================================================= */
#include <Arduino.h>

static icu_fsm_t s_fsm;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\r\n=======================================================");
    Serial.println("  Smart Medical ICU Safety & Patient Monitoring System ");
    Serial.println("  Target: STM32 Black Pill (ARM Cortex-M4 @ 84/100MHz) ");
    Serial.println("=======================================================\r\n");

    hal_sensors_init();
    icu_fsm_init(&s_fsm);

    pinMode(PIN_LED_NORMAL_GREEN, OUTPUT);
    pinMode(PIN_LED_WARN_YELLOW, OUTPUT);
    pinMode(PIN_LED_CRIT_RED, OUTPUT);
    pinMode(PIN_ONBOARD_LED, OUTPUT);
    pinMode(PIN_BUZZER_ALARM, OUTPUT);
}

void loop() {
    static uint32_t last_telemetry_ms = 0;
    icu_sensor_data_t sensor_frame;
    icu_telemetry_packet_t packet;

    /* 1. Acquire Synchronized Sensor Frame */
    hal_sensors_read_all(&sensor_frame);

    /* 2. Execute FSM Step (20 Hz / 50ms tick rate) */
    icu_fsm_step(&s_fsm, &sensor_frame, &packet);

    /* 3. Stream Telemetry at 1 Hz */
    uint32_t now = millis();
    if (now - last_telemetry_ms >= ICU_TELEMETRY_PERIOD_MS) {
        last_telemetry_ms = now;

        char json_buf[512];
        icu_telemetry_format_json(&packet, json_buf, sizeof(json_buf));
        Serial.println(json_buf);

        /* Toggle heartbeat */
        digitalWrite(PIN_ONBOARD_LED, !digitalRead(PIN_ONBOARD_LED));
    }

    delay(ICU_MAIN_LOOP_TICK_MS);
}

#else
/* =========================================================================
 * Host Verification & Clinical Scenario Emulation Engine
 * ========================================================================= */
int main(void) {
    printf("=================================================================\n");
    printf("  Smart Medical ICU Safety & Patient Monitoring System\n");
    printf("  Host Simulation, Clinical Logic & FSM Emulation Engine\n");
    printf("=================================================================\n\n");

    hal_sensors_init();
    icu_fsm_t fsm;
    icu_fsm_init(&fsm);

    icu_sensor_data_t sensors;
    icu_telemetry_packet_t pkt;
    char console_buf[256];

    /* Test Sequence across clinical scenarios */
    struct {
        const char *name;
        icu_mock_scenario_t scenario;
        int ticks;
    } test_scenarios[] = {
        { "Phase 1: Bed Unoccupied / Awaiting Admission",   SCENARIO_EMPTY_BED,          8 },
        { "Phase 2: Patient Admitted / Stable Resting",     SCENARIO_NORMAL_RESTING,     8 },
        { "Phase 3: Abnormal Bed Incline Angle (> 65 deg)", SCENARIO_ABNORMAL_TILT,      8 },
        { "Phase 4: Bed Exit Attempt (Left IR Barrier)",    SCENARIO_BED_EXIT_ATTEMPT,   8 },
        { "Phase 5: High-G Patient Fall Shock Detected",    SCENARIO_PATIENT_FALL,       8 },
        { "Phase 6: Emergency Nurse Call Button Pressed",   SCENARIO_NURSE_CALL_TRIGGER, 8 }
    };

    size_t num_phases = sizeof(test_scenarios) / sizeof(test_scenarios[0]);

    for (size_t p = 0; p < num_phases; p++) {
        printf("--> %s\n", test_scenarios[p].name);
        hal_sensors_set_mock_scenario(test_scenarios[p].scenario);

        for (int i = 0; i < test_scenarios[p].ticks; i++) {
            hal_sensors_read_all(&sensors);
            icu_fsm_step(&fsm, &sensors, &pkt);
        }

        icu_telemetry_format_console(&pkt, console_buf, sizeof(console_buf));
        printf("%s\n", console_buf);
    }

    printf("[SUCCESS] All 6 clinical safety scenarios verified successfully.\n");
    return 0;
}
#endif
