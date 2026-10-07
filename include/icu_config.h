/**
 * @file icu_config.h
 * @brief Hardware pin mapping for STM32 Black Pill (Cortex-M4), timing, and clinical thresholds.
 * @author Harshavardhan (Smart Medical ICU Safety & Patient Monitoring System)
 * @license MIT
 */

#ifndef ICU_CONFIG_H
#define ICU_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * STM32 Black Pill (STM32F401CCU6 / STM32F411CEU6) Pin Mapping
 * ========================================================================= */
/* I2C1 Bus (MPU6050 IMU & SSD1306 OLED Display) */
#define PIN_I2C1_SCL                   PB8      ///< I2C1 Clock (400 kHz Fast-Mode)
#define PIN_I2C1_SDA                   PB9      ///< I2C1 Data (400 kHz Fast-Mode)

/* Ultrasonic Sensor (HC-SR04) */
#define PIN_ULTRASONIC_TRIG            PA1      ///< Trigger output pulse (10us)
#define PIN_ULTRASONIC_ECHO            PA2      ///< Echo input timer capture / pulse duration

/* Analog Ambient Light Sensor (LDR) */
#define PIN_LDR_ADC_IN                 PA0      ///< ADC1 Channel 0 (12-bit resolution)

/* Boundary Infrared Obstacle Sensors */
#define PIN_IR_LEFT_EDGE               PA3      ///< Digital Input (Active Low / Barrier trip)
#define PIN_IR_RIGHT_EDGE              PA4      ///< Digital Input (Active Low / Barrier trip)

/* Emergency Controls & Actuators */
#define PIN_EMERGENCY_SWITCH           PA5      ///< EXTI5 External Interrupt (Pull-up)
#define PIN_BUZZER_ALARM               PB0      ///< TIM3 Ch3 PWM / GPIO Active Buzzer
#define PIN_LED_NORMAL_GREEN           PB1      ///< Normal / Patient Safe indicator
#define PIN_LED_WARN_YELLOW            PB12     ///< Advisory / Warning indicator
#define PIN_LED_CRIT_RED               PB13     ///< Critical Alarm / Fall / Emergency

/* Onboard Black Pill LED */
#define PIN_ONBOARD_LED                PC13     ///< Active Low heartbeat LED

/* UART Console (USART1) */
#define PIN_UART1_TX                   PA9      ///< 115200 Baud Telemetry Stream
#define PIN_UART1_RX                   PA10

/* =========================================================================
 * Clinical & Sensor Algorithm Thresholds
 * ========================================================================= */
#define ICU_PATIENT_MIN_DIST_CM        10.0f    ///< Closest physical patient mattress proximity
#define ICU_PATIENT_MAX_DIST_CM        85.0f    ///< Beyond 85cm implies patient is out of bed
#define ICU_BED_VACANT_DEBOUNCE_CYCLES 5U       ///< Consecutive cycles to confirm vacant bed

/* Posture & Bed Incline (Fowler's Position Limits) */
#define ICU_BED_TILT_SAFE_MAX_DEG      60.0f    ///< Maximum recommended head incline
#define ICU_BED_TILT_SAFE_MIN_DEG      -10.0f   ///< Trendelenburg tilt limit

/* Impact & Fall Detection */
#define ICU_FALL_IMPACT_THRESHOLD_G    2.60f    ///< Peak acceleration during sudden drop / fall
#define ICU_FALL_SETTLE_WINDOW_MS      800U     ///< Settle window after shock to verify posture

/* Ambient Lux Limits */
#define ICU_LDR_NIGHT_THRESHOLD_ADC    800U     ///< ADC reading below 800 corresponds to dark room

/* Sampling Rates */
#define ICU_MAIN_LOOP_TICK_MS          50U      ///< 20 Hz cooperative scheduler tick rate
#define ICU_TELEMETRY_PERIOD_MS        1000U    ///< 1 Hz structured telemetry broadcast

#ifdef __cplusplus
}
#endif

#endif /* ICU_CONFIG_H */
