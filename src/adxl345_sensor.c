#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <logging/log.h>
#include <init.h>

LOG_MODULE_REGISTER(adxl345);

static int adxl345_init(const struct device *dev) {
    const struct device *sensor = device_get_binding("ADXL345");

    if (!sensor) {
        LOG_ERR("No device found for ADXL345");
        return -ENODEV;
    }

    LOG_INF("ADXL345 initialized");

    return 0;
}

SYS_INIT(adxl345_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);