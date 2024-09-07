#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#define MY_SERIAL DT_NODELABEL(adxl345)

LOG_MODULE_REGISTER(adxl345);

static const struct device *adxl345_dev;

// Function to read and log data
static void adxl345_read_data(struct k_work *work) {
    struct sensor_value accel_x, accel_y, accel_z;
    int ret;

    ret = sensor_sample_fetch(adxl345_dev);
    if (ret) {
        LOG_ERR("Failed to fetch sample from ADXL345: %d", ret);
        return;
    }

    ret = sensor_channel_get(adxl345_dev, SENSOR_CHAN_ACCEL_X, &accel_x);
    if (ret) {
        LOG_ERR("Failed to get X-axis data: %d", ret);
        return;
    }

    ret = sensor_channel_get(adxl345_dev, SENSOR_CHAN_ACCEL_Y, &accel_y);
    if (ret) {
        LOG_ERR("Failed to get Y-axis data: %d", ret);
        return;
    }

    ret = sensor_channel_get(adxl345_dev, SENSOR_CHAN_ACCEL_Z, &accel_z);
    if (ret) {
        LOG_ERR("Failed to get Z-axis data: %d", ret);
        return;
    }

    LOG_INF("ADXL345 Acceleration: X: %d.%06d, Y: %d.%06d, Z: %d.%06d",
            accel_x.val1, accel_x.val2,
            accel_y.val1, accel_y.val2,
            accel_z.val1, accel_z.val2);
}

// Work item structure for scheduling periodic data reads
K_WORK_DELAYABLE_DEFINE(adxl345_work, adxl345_read_data);

// Initialize the ADXL345 and schedule periodic reads
static int adxl345_init(const struct device *dev) {
    adxl345_dev = DEVICE_DT_GET(MY_SERIAL);

    if (!adxl345_dev) {
        LOG_ERR("No device found for ADXL345");
        return -ENODEV;
    }

    if (!device_is_ready(adxl345_dev)) {
        LOG_ERR("Device not ready for ADXL345");
        return -ENODEV;
    }

    LOG_INF("ADXL345 initialized");

    // Schedule the work item to run every second
    k_work_schedule(&adxl345_work, K_SECONDS(1));

    return 0;
}

SYS_INIT(adxl345_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);