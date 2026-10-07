cmake_minimum_required(VERSION 3.16)
project(SmartICUMonitor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(
    include
    tests
)

# Common Core Sources
set(ICU_CORE_SRCS
    src/core/icu_fsm.c
    src/core/patient_monitor.c
    src/hal/hal_sensors.c
    src/telemetry/icu_telemetry.c
)

# Host Simulation Executable
add_executable(icu_simulator
    src/main.c
    ${ICU_CORE_SRCS}
)
target_link_libraries(icu_simulator m)

# Unit Tests
enable_testing()

add_executable(test_fsm tests/test_fsm.c ${ICU_CORE_SRCS})
target_link_libraries(test_fsm m)
add_test(NAME TestFSM COMMAND test_fsm)

add_executable(test_patient_monitor tests/test_patient_monitor.c src/core/patient_monitor.c)
target_link_libraries(test_patient_monitor m)
add_test(NAME TestPatientMonitor COMMAND test_patient_monitor)
