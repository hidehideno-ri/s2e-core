/**
 * @file InitAttitude.hpp
 * @brief Initialize function for attitude
 */
#pragma once

#include "Attitude.h"
#include "AttitudeRK4.h"
#include "ControlledAttitude.h"

/**
 * @fn InitAttitude
 * @brief Initialize function for Attitude
 * @param [in] file_name: Path to the initialize file
 * @param [in] orbit: Orbit information
 * @param [in] celes_info: Celestial information
 * @param [in] step_sec: Step width [sec]
 * @param [in] inertia_tensor: Inertia tensor [kg m^2]
 * @param [in] sat_id: Satellite ID
 */
Attitude* InitAttitude(std::string file_name, const Orbit* orbit, const LocalCelestialInformation* celes_info, const double step_sec,
                       const Matrix<3, 3> inertia_tensor, const int sat_id);
