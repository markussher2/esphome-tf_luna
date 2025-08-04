#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace tf_luna {

class TFLunaUARTSensor : public PollingComponent,
                         public uart::UARTDevice,
                         public sensor::Sensor {
 public:
  // Constructor takes the UART bus handle
  explicit TFLunaUARTSensor(uart::UARTComponent *parent)
    : UARTDevice(parent),
      PollingComponent(1000) {}  // default 1000 ms, overridden by YAML

  void setup() override;
  void update() override;

  // Setter for optional signal strength sensor
  void set_signal_sensor(sensor::Sensor *signal_sensor) { signal_sensor_ = signal_sensor; }

 protected:
  static constexpr size_t FRAME_SIZE = 9;
  uint8_t buffer[FRAME_SIZE];

  sensor::Sensor *signal_sensor_{nullptr};
};

}  // namespace tf_luna
}  // namespace esphome
