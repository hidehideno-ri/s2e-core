/**
 * @file SampleComponents.cpp
 * @brief An example of user side components management installed on a spacecraft
 */

#include "SampleComponents.h"

#include <Interface/InitInput/IniAccess.h>

#include "Sample_PortConfig.h"

SampleComponents::SampleComponents(const Dynamics* dynamics, const Structure* structure, const LocalEnvironment* local_env,
                                   const GlobalEnvironment* glo_env, const SimulationConfig* config, ClockGenerator* clock_gen, const int sat_id)
    : config_(config), dynamics_(dynamics), structure_(structure), local_env_(local_env), glo_env_(glo_env) {
  IniAccess iniAccess = IniAccess(config_->sat_file_[sat_id]);

  // PCU power port connection
  pcu_ = new PCU(clock_gen);
  pcu_->ConnectPort(0, 0.5, 3.3, 1.0);  // OBC: assumed power consumption is defined here
  pcu_->ConnectPort(1, 1.0);            // Gyro: assumed power consumption is defined inside the InitGyro
  pcu_->ConnectPort(2, 1.0);            // for other all components

  // Components
  obc_ = new OBC(1, clock_gen, pcu_->GetPowerPort(0));
  hils_port_manager_ = new HilsPortManager();

  // Gyro
  std::string ini_path = iniAccess.ReadString("COMPONENTS_FILE", "gyro_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  gyro_ = new Gyro(InitGyro(clock_gen, pcu_->GetPowerPort(1), 1, ini_path, glo_env_->GetSimTime().GetCompoStepSec(), dynamics_));

  // MagSensor
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "mag_sensor_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  mag_sensor_ =
      new MagSensor(InitMagSensor(clock_gen, pcu_->GetPowerPort(2), 1, ini_path, glo_env_->GetSimTime().GetCompoStepSec(), &(local_env_->GetMag())));

  // STT
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "stt_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  stt_ = new STT(InitSTT(clock_gen, pcu_->GetPowerPort(2), 1, ini_path, glo_env_->GetSimTime().GetCompoStepSec(), dynamics_, local_env_));

  // SunSensor
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "ss_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  sun_sensor_ = new SunSensor(InitSunSensor(clock_gen, pcu_->GetPowerPort(2), 1, ini_path, &(local_env_->GetSrp()), &(local_env_->GetCelesInfo())));

  // GNSS-R
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "gnss_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  gnss_ = new GNSSReceiver(
      InitGNSSReceiver(clock_gen, pcu_->GetPowerPort(2), 1, ini_path, dynamics_, &(glo_env_->GetGnssSatellites()), &(glo_env_->GetSimTime())));

  // MagTorquer
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "mag_torquer_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  mag_torquer_ = new MagTorquer(
      InitMagTorquer(clock_gen, pcu_->GetPowerPort(2), 1, ini_path, glo_env_->GetSimTime().GetCompoStepSec(), &(local_env_->GetMag())));

  // RW
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "rw_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  rw_ = new RWModel(
      InitRWModel(clock_gen, pcu_->GetPowerPort(2), 1, ini_path, dynamics_->GetAttitude().GetPropStep(), glo_env_->GetSimTime().GetCompoStepSec()));

  // Thruster
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "thruster_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  thruster_ = new SimpleThruster(InitSimpleThruster(clock_gen, pcu_->GetPowerPort(2), 1, ini_path, structure_, dynamics));

  // Force Generator
  ini_path = iniAccess.ReadString("COMPONENTS_FILE", "force_generator_file");
  config_->main_logger_->CopyFileToLogDir(ini_path);
  force_generator_ = new ForceGenerator(InitializeForceGenerator(clock_gen, ini_path, dynamics_));

  // PCU power port initial control
  pcu_->GetPowerPort(0)->SetVoltage(3.3);
  pcu_->GetPowerPort(1)->SetVoltage(3.3);
  pcu_->GetPowerPort(2)->SetVoltage(3.3);

  // Examples of HILS
  // UART tutorial. Comment out when not in use.
  // exp_hils_uart_responder_ = new ExpHils(clock_gen, 1, obc_, 3, 9600,
  // hils_port_manager_, 1); exp_hils_uart_sender_ = new ExpHils(clock_gen, 0,
  // obc_, 4, 9600, hils_port_manager_, 0);
  // I2C tutorial. Comment out when not in use.
  // exp_hils_i2c_controller_ = new ExpHilsI2cController(
  // 30, clock_gen, 5, 115200, 256, 256, hils_port_manager_);
  // exp_hils_i2c_target_ =
  // new ExpHilsI2cTarget(1, clock_gen, 0, 0x44, obc_, 6, hils_port_manager_);

  // actuator debug output
  // libra::Vector<kMtqDim> mag_moment_c{0.01};
  // mag_torquer_->SetMagMomentC(mag_moment_c);
  // rw_->SetTargetTorqueRw(0.01);
  // rw_->SetDriveFlag(true);
  // thruster_->SetDuty(0.9);

  // force generator debug output
  // libra::Vector<3> force_N;
  // force_N[0] = 1.0;
  // force_N[1] = 0.0;
  // force_N[2] = 0.0;
  // force_generator_->SetForce_b_N(force_N);
}

SampleComponents::~SampleComponents() {
  delete gyro_;
  delete mag_sensor_;
  delete stt_;
  delete sun_sensor_;
  delete gnss_;
  delete mag_torquer_;
  delete rw_;
  delete thruster_;
  delete force_generator_;
  delete pcu_;
  // delete exp_hils_uart_responder_;
  // delete exp_hils_uart_sender_;
  // delete exp_hils_i2c_controller_;
  // delete exp_hils_i2c_target_;
  delete obc_;
  delete hils_port_manager_;  // delete after exp_hils
}

libra::Vector<3> SampleComponents::GenerateForce_N_b() {
  libra::Vector<3> force_N_b_(0.0);
  force_N_b_ += thruster_->GetThrustB();
  force_N_b_ += force_generator_->GetGeneratedForce_b_N();
  return force_N_b_;
}

libra::Vector<3> SampleComponents::GenerateTorque_Nm_b() {
  libra::Vector<3> torque_Nm_b_(0.0);
  torque_Nm_b_ += mag_torquer_->GetTorque_b();
  torque_Nm_b_ += rw_->GetOutputTorqueB();
  torque_Nm_b_ += thruster_->GetTorqueB();
  return torque_Nm_b_;
}

void SampleComponents::LogSetup(Logger& logger) {
  logger.AddLoggable(gyro_);
  logger.AddLoggable(mag_sensor_);
  logger.AddLoggable(stt_);
  logger.AddLoggable(sun_sensor_);
  logger.AddLoggable(gnss_);
  logger.AddLoggable(mag_torquer_);
  logger.AddLoggable(rw_);
  logger.AddLoggable(thruster_);
  logger.AddLoggable(force_generator_);
}
