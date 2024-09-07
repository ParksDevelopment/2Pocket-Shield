#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

#define MY_SERIAL DT_NODELABEL(adxl345)

LOG_MODULE_REGISTER(adxl345);

static void adxl345_read_data(const struct device *sensor) {
    struct sensor_value accel_x, accel_y, accel_z;
    int ret;

    ret = sensor_sample_fetch(sensor);
    if (ret) {
        LOG_ERR("Failed to fetch sample from ADXL345: %d", ret);
        return;
    }

    ret = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_X, &accel_x);
    if (ret) {
        LOG_ERR("Failed to get X-axis data: %d", ret);
        return;
    }

    ret = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_Y, &accel_y);
    if (ret) {
        LOG_ERR("Failed to get Y-axis data: %d", ret);
        return;
    }

    ret = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_Z, &accel_z);
    if (ret) {
        LOG_ERR("Failed to get Z-axis data: %d", ret);
        return;
    }

    LOG_INF("ADXL345 Acceleration: X: %d.%06d, Y: %d.%06d, Z: %d.%06d",
            accel_x.val1, accel_x.val2,
            accel_y.val1, accel_y.val2,
            accel_z.val1, accel_z.val2);
}

static void list_i2c_devices(void) {
    const struct device *i2c_dev;
    struct device *dev;
    int i2c_count = 0;

    // Iterate over all devices
    for (i2c_count = 0; i2c_count < DEVICE_MAX; i2c_count++) {
        dev = device_get_binding(i2c_count);
        if (dev) {
            if (device_is_ready(dev) && device_is_powered(dev) && device_is_enabled(dev)) {
                LOG_INF("I2C Device found: %s", log_strdup(device_get_name(dev)));
            }
        }
    }
}

static int adxl345_init(const struct device *dev) {
    const struct device *sensor = DEVICE_DT_GET(MY_SERIAL);

    if (!sensor) {
        LOG_ERR("No device found for ADXL345");
        return -ENODEV;
    }

    if (!device_is_ready(sensor)) {
        LOG_ERR("ADXL345 is not ready");
        return -ENODEV;
    }

    LOG_INF("ADXL345 initialized");

    // Call the read function right after initialization to get a single reading
    adxl345_read_data(sensor);

    // List all I2C devices
    list_i2c_devices();

    return 0;
}

SYS_INIT(adxl345_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
