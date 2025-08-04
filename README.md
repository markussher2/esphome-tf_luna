# esphome-tf_luna
Custom external components for esphome integration with TF_Luna Lidar distance sensor

Example esphome YAML configuration:
```yaml
external_components:
  - source:
      type: local
      path: ./components               # points to /config/esphome
    components: [tf_luna]   # matches the folder name under components/

uart:
  id: uart_bus_1
  tx_pin: GPIO6
  rx_pin: GPIO7
  baud_rate: 115200  

sensor:
  - platform: tf_luna
    uart_id: uart_bus_1
    name: "TF-Luna Distance"
    id: tf_luna_distance
    unit_of_measurement: "m"
    accuracy_decimals: 2
    device_class: "distance"
    update_interval: 10s

    signal_strength:
      name: "TF-Luna Signal Strength"
      accuracy_decimals: 0
      device_class: "signal_strength"
      unit_of_measurement: ""
```
Always check you have the correct GPIO pinouts:
https://www.waveshare.com/wiki/TF-Luna_LiDAR_Range_Sensor
