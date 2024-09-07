#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

LOG_MODULE_REGISTER(adxl345);

static void list_devices(void) {
    LOG_INF("Listing all devices:");
    const struct device *dev;
    for (const struct device *dev = NULL; (dev = device_list_get_next(dev));) {
        LOG_INF("Device: %s", dev->name);
    }
}

static int adxl345_init(const struct device *dev) {
    // List all available devices for debugging purposes
    list_devices();

    // Try to get a binding to the ADXL345 sensor
    const struct device *sensor = device_get_binding("ADXL345");

    // Log more details if sensor is not found
    if (!sensor) {
        LOG_ERR("No device found for ADXL345");

        // Check if ADXL345 is in the device tree by listing the available devices
        list_devices();

        return -ENODEV;
    }

    LOG_INF("ADXL345 initialized successfully");

    return 0;
}

// Register the initialization function with SYS_INIT
SYS_INIT(adxl345_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
