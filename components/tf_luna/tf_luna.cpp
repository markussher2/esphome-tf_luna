#include "tf_luna.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tf_luna {

static const char *const TAG = "tf_luna";
static constexpr size_t FRAME_SIZE = 9;
static constexpr uint8_t HEADER_BYTE = 0x59;
static constexpr uint8_t MIN_SIGNAL_STRENGTH = 10;

void TFLunaUARTSensor::setup() {
//  ESP_LOGCONFIG(TAG, "Setting up TF-Luna UART sensor...");
}

void TFLunaUARTSensor::update() {
//  ESP_LOGD(TAG, "update() called");

  // 🧹 Flush excess buffer to avoid stale frames
  int excess_bytes = this->available() - FRAME_SIZE * 2;
  if (excess_bytes > 0) {
//    ESP_LOGD(TAG, "Flushing %d excess bytes from UART buffer", excess_bytes);
    for (int i = 0; i < excess_bytes; i++) {
      this->read();  // discard
    }
  }

  float latest_distance = NAN;
  uint16_t latest_signal = 0;
  uint8_t latest_frame[FRAME_SIZE] = {0};

  // 🔁 Scan for latest valid frame
  while (this->available() >= FRAME_SIZE) {
    uint8_t first = this->read();
    if (first != HEADER_BYTE) continue;

    uint8_t second = this->peek();
    if (second != HEADER_BYTE) continue;

    this->read();  // consume second 0x59

    uint8_t buffer[FRAME_SIZE] = {HEADER_BYTE, HEADER_BYTE};
    this->read_array(&buffer[2], FRAME_SIZE - 2);

    // ✅ Checksum validation
    uint8_t checksum = 0;
    for (size_t i = 0; i < 8; i++) checksum += buffer[i];
    if (checksum != buffer[8]) {
//      ESP_LOGW(TAG, "Checksum mismatch: expected %02X, got %02X", checksum, buffer[8]);
      continue;
    }

    // ✅ Parse distance and signal strength
    uint16_t distance_raw = buffer[2] | (buffer[3] << 8);
    uint16_t signal_raw = buffer[4] | (buffer[5] << 8);

    if (signal_raw < MIN_SIGNAL_STRENGTH) {
      ESP_LOGW(TAG, "Weak signal (%d), skipping frame", signal_raw);
      continue;
    }

    latest_distance = distance_raw / 100.0f;
    latest_signal = signal_raw;
    memcpy(latest_frame, buffer, FRAME_SIZE);  // save for logging
  }

  // 📤 Publish latest valid frame
  if (!std::isnan(latest_distance)) {
//    ESP_LOGD(TAG, "Latest valid frame: %02X %02X %02X %02X %02X %02X %02X %02X %02X",
//             latest_frame[0], latest_frame[1], latest_frame[2], latest_frame[3],
//             latest_frame[4], latest_frame[5], latest_frame[6], latest_frame[7],
//             latest_frame[8]);

//    ESP_LOGD(TAG, "Published distance: %.2f m, signal strength: %d", latest_distance, latest_signal);
    this->publish_state(latest_distance);

    if (signal_sensor_ != nullptr) {
      signal_sensor_->publish_state(latest_signal);
    }
  } else {
    ESP_LOGD(TAG, "No valid frame with strong signal found");
  }
}

}  // namespace tf_luna
}  // namespace esphome
