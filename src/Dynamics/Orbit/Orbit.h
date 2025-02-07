/**
 * @file Orbit.h
 * @brief Base class of orbit propagation
 */
#ifndef __orbit_H__
#define __orbit_H__

#include <Library/math/Constant.hpp>
#include <Library/math/MatVec.hpp>
#include <Library/math/Matrix.hpp>
#include <Library/math/Quaternion.hpp>
#include <Library/math/Vector.hpp>

using libra::Matrix;
using libra::Quaternion;
using libra::Vector;

#include <Environment/Global/CelestialInformation.h>
#include <Interface/LogOutput/ILoggable.h>

#include <Environment/Global/PhysicalConstants.hpp>
#include <Library/Geodesy/GeodeticPosition.hpp>

/**
 * @class Orbit
 * @brief Base class of orbit propagation
 */
class Orbit : public ILoggable {
 public:
  /**
   * @fn Orbit
   * @brief Constructor
   * @param [in] celes_info: Celestial information
   */
  Orbit(const CelestialInformation* celes_info) : celes_info_(celes_info) {}
  /**
   * @fn ~Orbit
   * @brief Destructor
   */
  virtual ~Orbit() {}

  /**
   * @enum PROPAGATE_MODE
   * @brief Propagation mode of orbit
   */
  enum class PROPAGATE_MODE { RK4 = 0, SGP4, RELATIVE_ORBIT, KEPLER, ENCKE };

  /**
   * @fn Propagate
   * @brief Pure virtual function for orbit propagation
   * @param [in] endtime: End time of simulation [sec]
   * @param [in] current_jd: Current Julian day [day]
   */
  virtual void Propagate(double endtime, double current_jd) = 0;

  /**
   * @fn UpdateAtt
   * @brief Update attitude information
   * @param [in] q_i2b: End time of simulation [sec]
   */
  inline void UpdateAtt(Quaternion q_i2b) { sat_velocity_b_ = q_i2b.frame_conv(sat_velocity_i_); }

  /**
   * @fn AddPositionOffset
   * @brief Shift the position of the spacecraft
   * @note Is this really needed?
   * @param [in] offset_i: Offset vector in the inertial frame [m]
   */
  inline virtual void AddPositionOffset(Vector<3> offset_i) { (void)offset_i; }

  // Getters
  /**
   * @fn GetIsCalcEnabled
   * @brief Return calculate flag
   */
  inline bool GetIsCalcEnabled() const { return is_calc_enabled_; }
  /**
   * @fn GetPropagateMode
   * @brief Return propagate mode
   */
  inline PROPAGATE_MODE GetPropagateMode() const { return propagate_mode_; }
  /**
   * @fn GetSatPosition_i
   * @brief Return spacecraft position in the inertial frame [m]
   */
  inline Vector<3> GetSatPosition_i() const { return sat_position_i_; }
  /**
   * @fn GetSatPosition_ecef
   * @brief Return spacecraft position in the ECEF frame [m]
   */
  inline Vector<3> GetSatPosition_ecef() const { return sat_position_ecef_; }
  /**
   * @fn GetSatVelocity_i
   * @brief Return spacecraft velocity in the inertial frame [m/s]
   */
  inline Vector<3> GetSatVelocity_i() const { return sat_velocity_i_; }
  /**
   * @fn GetSatVelocity_b
   * @brief Return spacecraft velocity in the body fixed frame [m/s]
   */
  inline Vector<3> GetSatVelocity_b() const { return sat_velocity_b_; }
  /**
   * @fn GetSatVelocity_ecef
   * @brief Return spacecraft velocity in the ECEF frame [m/s]
   */
  inline Vector<3> GetSatVelocity_ecef() const { return sat_velocity_ecef_; }
  /**
   * @fn GetGeodeticPosition
   * @brief Return spacecraft position in the geodetic frame [m]
   */
  inline GeodeticPosition GetGeodeticPosition() const { return sat_position_geo_; }

  // TODO delete the following functions
  inline double GetLat_rad() const { return sat_position_geo_.GetLat_rad(); }
  inline double GetLon_rad() const { return sat_position_geo_.GetLon_rad(); }
  inline double GetAlt_m() const { return sat_position_geo_.GetAlt_m(); }
  inline Vector<3> GetLatLonAlt() const {
    Vector<3> vec;
    vec(0) = sat_position_geo_.GetLat_rad();
    vec(1) = sat_position_geo_.GetLon_rad();
    vec(2) = sat_position_geo_.GetAlt_m();
    return vec;
  }

  // Setters
  /**
   * @fn SetIsCalcEnabled
   * @brief Set calculate flag
   */
  inline void SetIsCalcEnabled(bool is_calc_enabled) { is_calc_enabled_ = is_calc_enabled; }
  /**
   * @fn SetAcceleration_i
   * @brief Set acceleration in the inertial frame [m/s2]
   */
  inline void SetAcceleration_i(Vector<3> acceleration_i) { acc_i_ = acceleration_i; }
  /**
   * @fn AddForce_i
   * @brief Add force
   * @param [in] force_i: Force in the inertial frame [N]
   * @param [in] spacecraft_mass: Mass of spacecraft [kg]
   */
  inline void AddForce_i(Vector<3> force_i, double spacecraft_mass) {
    force_i /= spacecraft_mass;
    acc_i_ += force_i;
  }
  /**
   * @fn AddAcceleration_i
   * @brief Add acceleration in the inertial frame [m/s2]
   */
  inline void AddAcceleration_i(Vector<3> acceleration_i) { acc_i_ += acceleration_i; }
  /**
   * @fn AddForce_i
   * @brief Add force
   * @param [in] force_b: Force in the body fixed frame [N]
   * @param [in] q_i2b: Quaternion from the inertial frame to the body fixed frame
   * @param [in] spacecraft_mass: Mass of spacecraft [kg]
   */
  inline void AddForce_b(Vector<3> force_b, Quaternion q_i2b, double spacecraft_mass) {
    auto force_i = q_i2b.frame_conv_inv(force_b);
    AddForce_i(force_i, spacecraft_mass);
  }

  /**
   * @fn CalcQuaternionI2LVLH
   * @brief Calculate quaternion from the inertial frame to the LVLH frame
   */
  Quaternion CalcQuaternionI2LVLH() const;

  // Override ILoggable
  /**
   * @fn GetLogHeader
   * @brief Override GetLogHeader function of ILoggable
   */
  virtual std::string GetLogHeader() const = 0;
  /**
   * @fn GetLogValue
   * @brief Override GetLogValue function of ILoggable
   */
  virtual std::string GetLogValue() const = 0;

 protected:
  const CelestialInformation* celes_info_;  //!< Celestial information

  // Settings
  bool is_calc_enabled_ = false;   //!< Calculate flag
  PROPAGATE_MODE propagate_mode_;  //!< Propagation mode

  Vector<3> sat_position_i_;           //!< Spacecraft position in the inertial frame [m]
  Vector<3> sat_position_ecef_;        //!< Spacecraft position in the ECEF frame [m]
  GeodeticPosition sat_position_geo_;  //!< Spacecraft position in the Geodetic frame

  Vector<3> sat_velocity_i_;     //!< Spacecraft velocity in the inertial frame [m/s]
  Vector<3> sat_velocity_b_;     //!< Spacecraft velocity in the body frame [m/s]
  Vector<3> sat_velocity_ecef_;  //!< Spacecraft velocity in the ECEF frame [m/s]

  Vector<3> acc_i_;  //!< Spacecraft acceleration in the inertial frame [m/s2]
                     //!< NOTE: Clear to zero at the end of the Propagate function

  // Frame Conversion TODO: consider other planet
  /**
   * @fn TransEciToEcef
   * @brief Transform states from the ECI frame to ECEF frame
   */
  void TransEciToEcef(void);
  /**
   * @fn TransEcefToGeo
   * @brief Transform states from the ECEF frame to the geodetic frame
   */
  void TransEcefToGeo(void);
};

#endif  //__orbit_H__
