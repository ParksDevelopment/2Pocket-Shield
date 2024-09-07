#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#define MY_SERIAL DT_NODELABEL(adxl345)

LOG_MODULE_REGISTER(adxl345);

static int adxl345_init(const struct device *dev) {
    const struct device *sensor = DEVICE_DT_GET(MY_SERIAL);

    if (!sensor) {
        LOG_ERR("No device found for ADXL345");
        return -ENODEV;
    }

    LOG_INF("ADXL345 initialized");

    return 0;
}

SYS_INIT(adxl345_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);