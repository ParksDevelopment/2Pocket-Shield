#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#define MY_SERIAL DT_NODELABEL(adxl345)

LOG_MODULE_REGISTER(adxl345);

// Timer callback function
void adxl345_timer_handler(struct k_timer *timer_id) {
    const struct device *sensor = DEVICE_DT_GET(MY_SERIAL);

    if (!sensor) {
        LOG_ERR("No device found for ADXL345");
        return;
    }

    if (!device_is_ready(sensor)) {
        LOG_ERR("ADXL345 is not ready");
        return;
    }

    adxl345_read_data(sensor);
}

// Timer definition
K_TIMER_DEFINE(adxl345_timer, adxl345_timer_handler, NULL);

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

    // Start the timer to call adxl345_read_data() every second
    k_timer_start(&adxl345_timer, K_SECONDS(1), K_SECONDS(1));

    return 0;
}

SYS_INIT(adxl345_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);