#include <iostream>

#include "ObcI2cControllerCommunicationBase.h"

ObcI2cControllerCommunicationBase::ObcI2cControllerCommunicationBase(
  const unsigned int hils_port_id,
  const unsigned int baud_rate,
  const unsigned int tx_buf_size,
  const unsigned int rx_buf_size,
  HilsPortManager* hils_port_manager)
: hils_port_id_(hils_port_id), baud_rate_(baud_rate), tx_buf_size_(tx_buf_size), rx_buf_size_(rx_buf_size), hils_port_manager_(hils_port_manager)
{
#ifdef USE_HILS
  sim_mode_ = OBC_COM_UART_MODE::HILS;
  int ret = hils_port_manager_->I2cControllerConnectComPort(hils_port_id_, baud_rate_, tx_buf_size_, rx_buf_size_);
  if (ret != 0)
  {
    std::cout << "Error: ObcI2cCommunication ConnectComPort ID:" << hils_port_id_ << "\n";
  }
#else
  sim_mode_ = OBC_COM_UART_MODE::MODE_ERROR;
  printf("Error: USE_HILS:OFF Check compo initialization");
#endif
}

ObcI2cControllerCommunicationBase::~ObcI2cControllerCommunicationBase()
{
  if (sim_mode_ != OBC_COM_UART_MODE::HILS) return;

  int ret = hils_port_manager_->I2cControllerCloseComPort(hils_port_id_);
  if (ret != 0)
  {
    std::cout << "Error: ObcI2cCommunication CloseComPort ID:" << hils_port_id_ << "\n";
  }
}

int ObcI2cControllerCommunicationBase::ReceiveTelemetry(const unsigned char len)
{
  if (sim_mode_ != OBC_COM_UART_MODE::HILS) return -1;
  return hils_port_manager_->I2cControllerReceive(hils_port_id_, &rx_buffer_.front(), 0, len);
}

int ObcI2cControllerCommunicationBase::SendCommand(const unsigned char len)
{
  if (sim_mode_ != OBC_COM_UART_MODE::HILS) return -1;
  return hils_port_manager_->I2cControllerSend(hils_port_id_, &tx_buffer_.front(), 0, len);
}
