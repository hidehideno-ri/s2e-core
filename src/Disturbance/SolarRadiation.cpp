/**
 * @file SolarRadiation.cpp
 * @brief Class to calculate the solar radiation pressure disturbance force and torque
 */

#include "SolarRadiation.h"

#include <cmath>

#include "../Interface/LogOutput/LogUtility.h"

SolarRadiation::SolarRadiation(const vector<Surface>& surfaces, const Vector<3>& cg_b) : SurfaceForce(surfaces, cg_b) {}

void SolarRadiation::Update(const LocalEnvironment& local_env, const Dynamics& dynamics) {
  UNUSED(dynamics);

  Vector<3> tmp = local_env.GetCelesInfo().GetPosFromSC_b("SUN");
  CalcTorqueForce(tmp, local_env.GetSrp().CalcTruePressure());
}

void SolarRadiation::CalcCoef(Vector<3>& input_b, double item) {
  UNUSED(input_b);

  for (size_t i = 0; i < surfaces_.size(); i++) {  // Calculate for each surface
    double area = surfaces_[i].GetArea();
    double reflectivity = surfaces_[i].GetReflectivity();
    double specularity = surfaces_[i].GetSpecularity();
    normal_coef_[i] =
        area * item * ((1.0 + reflectivity * specularity) * pow(cosX[i], 2.0) + 2.0 / 3.0 * reflectivity * (1.0 - specularity) * cosX[i]);
    tangential_coef_[i] = area * item * (1.0 - reflectivity * specularity) * cosX[i] * sinX[i];
  }
}

std::string SolarRadiation::GetLogHeader() const {
  std::string str_tmp = "";

  str_tmp += WriteVector("srp_torque", "b", "Nm", 3);
  str_tmp += WriteVector("srp_force", "b", "N", 3);

  return str_tmp;
}

std::string SolarRadiation::GetLogValue() const {
  std::string str_tmp = "";

  str_tmp += WriteVector(torque_b_);
  str_tmp += WriteVector(force_b_);

  return str_tmp;
}
