/**
 * @file SampleSat.h
 * @brief An example of user side spacecraft class
 */

#pragma once

#include "../Spacecraft.h"
#include "SampleComponents.h"

class SampleComponents;

/**
 * @class SampleSat
 * @brief An example of user side spacecraft class
 */
class SampleSat : public Spacecraft {
 public:
  /**
   * @fn SampleSat
   * @brief Constructor
   */
  SampleSat(SimulationConfig* sim_config, const GlobalEnvironment* glo_env, const int sat_id);
};
