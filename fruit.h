#include <SoftwareSerial.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"



class fruit {
public:
  fruit(int rx_pin, int tx_pin, int mode_pin, int cts_pin, int rts_pin)
    : m_ss{ nullptr }
    , m_ble{ nullptr }
    , m_is_init{ false }
  { 
    m_ss = new SoftwareSerial(rx_pin, tx_pin);
    m_ble = new Adafruit_BluefruitLE_UART(*m_ss, mode_pin, cts_pin, rts_pin);
  }

  virtual 
  ~fruit()  
  {
    if (m_ble != nullptr) {
      delete m_ble;
    }

    if (m_ss != nullptr) {
      delete m_ss;
    }
  }
  
  Adafruit_BluefruitLE_UART *
  uart() 
  {
    return m_ble;
  }

  void
  listen()
  {
    m_ss->listen();
  }

  bool
  is_init() 
  {
    return m_is_init;
  }

  void
  is_init(bool b)
  {
    m_is_init = b;
  }

private:
  SoftwareSerial *m_ss;
  Adafruit_BluefruitLE_UART *m_ble;
  bool m_is_init;
};
