/**
 * @file InitStt.hpp
 * @brief Initialize functions for star sensor
 */

#pragma once

#include <Component/AOCS/STT.h>

/**
 * @fn InitSTT
 * @brief Initialize functions for STT without power port
 * @param [in] clock_gen: Clock generator
 * @param [in] sensor_id: Sensor ID
 * @param [in] fname: Path to the initialize file
 * @param [in] compo_step_time: Component step time [sec]
 * @param [in] dynamics: Dynamics information
 * @param [in] local_env: Local environment information
 */
STT InitSTT(ClockGenerator* clock_gen, int sensor_id, const std::string fname, double compo_step_time, const Dynamics* dynamics,
            const LocalEnvironment* local_env);
/**
 * @fn InitSTT
 * @brief Initialize functions for STT with power port
 * @param [in] clock_gen: Clock generator
 * @param [in] power_port: Power port
 * @param [in] sensor_id: Sensor ID
 * @param [in] fname: Path to the initialize file
 * @param [in] compo_step_time: Component step time [sec]
 * @param [in] dynamics: Dynamics information
 * @param [in] local_env: Local environment information
 */
STT InitSTT(ClockGenerator* clock_gen, PowerPort* power_port, int sensor_id, const std::string fname, double compo_step_time,
            const Dynamics* dynamics, const LocalEnvironment* local_env);
