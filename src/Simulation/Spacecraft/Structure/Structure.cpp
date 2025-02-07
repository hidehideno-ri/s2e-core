/**
 * @file Structure.cpp
 * @brief Definition of spacecraft structure
 */

#include "Structure.h"

#include <Interface/InitInput/IniAccess.h>

#include <Simulation/Spacecraft/Structure/InitStructure.hpp>

Structure::Structure(SimulationConfig* sim_config, const int sat_id) { Initialize(sim_config, sat_id); }

Structure::~Structure() {
  delete kinnematics_params_;
  delete rmm_params_;
}

void Structure::Initialize(SimulationConfig* sim_config, const int sat_id) {
  // Read file name
  IniAccess conf = IniAccess(sim_config->sat_file_[sat_id]);
  std::string ini_fname = conf.ReadString("STRUCTURE_FILE", "structure_file");
  // Save ini file
  sim_config->main_logger_->CopyFileToLogDir(ini_fname);
  // Initialize
  kinnematics_params_ = new KinematicsParams(InitKinematicsParams(ini_fname));
  surfaces_ = InitSurfaces(ini_fname);
  rmm_params_ = new RMMParams(InitRMMParams(ini_fname));
}
