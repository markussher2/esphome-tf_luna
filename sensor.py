# components/tf_luna/sensor.py

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

DEPENDENCIES = ["uart"]
CONF_UART_ID = "uart_id"
CONF_SIGNAL_STRENGTH = "signal_strength"

tf_luna_ns = cg.esphome_ns.namespace("tf_luna")
TFLunaUARTSensor = tf_luna_ns.class_("TFLunaUARTSensor", sensor.Sensor, cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = sensor.sensor_schema(TFLunaUARTSensor).extend({
    cv.GenerateID(CONF_ID): cv.declare_id(TFLunaUARTSensor),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
    cv.Optional(CONF_UPDATE_INTERVAL, default="1.0s"): cv.update_interval,
    cv.Optional(CONF_SIGNAL_STRENGTH): sensor.sensor_schema(),
})

async def to_code(config):
    uart_comp = await cg.get_variable(config[CONF_UART_ID])
    var = cg.new_Pvariable(config[CONF_ID], uart_comp)

    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))

    if CONF_SIGNAL_STRENGTH in config:
        sig = await sensor.new_sensor(config[CONF_SIGNAL_STRENGTH])
        cg.add(var.set_signal_sensor(sig))
