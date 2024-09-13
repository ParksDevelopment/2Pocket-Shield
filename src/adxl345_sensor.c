#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#define MY_SERIAL DT_NODELABEL(adxl345)
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5
#define READ_INTERVAL K_SECONDS(1)  // Adjust the interval as needed

LOG_MODULE_REGISTER(adxl345);

static const struct device *sensor;

static void adxl345_read_data(const struct device *sensor) {
    
    int attempts = 3;
    int ret;
    while (attempts > 0)
    {
        ret = sensor_sample_fetch(sensor);
        if (ret) {
            attempts -= 1;
        }
        else
        {
            LOG_INF("ADXL345 Acceleration: X: %d.%06d, Y: %d.%06d, Z: %d.%06d",
            accel_x.val1, accel_x.val2,
            accel_y.val1, accel_y.val2,
            accel_z.val1, accel_z.val2);
            attempts = 0;
        }
    }
}

static void adxl345_thread(void) {
    while (1) {
        adxl345_read_data(sensor);
        k_sleep(READ_INTERVAL);
    }
}

static int adxl345_init(const struct device *dev) {
    sensor = DEVICE_DT_GET(MY_SERIAL);

    if (!sensor) {
        LOG_ERR("No device found for ADXL345");
        return -ENODEV;
    }

    if (!device_is_ready(sensor)) {
        LOG_ERR("ADXL345 is not ready");
        return -ENODEV;
    }

    LOG_INF("ADXL345 initialized");

    // Create a thread that will periodically call the read function
    static struct k_thread adxl345_thread_data;
    static K_THREAD_STACK_DEFINE(adxl345_thread_stack, STACK_SIZE);

    k_thread_create(&adxl345_thread_data, adxl345_thread_stack,
                    K_THREAD_STACK_SIZEOF(adxl345_thread_stack),
                    (k_thread_entry_t) adxl345_thread,
                    NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    return 0;
}

SYS_INIT(adxl345_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);