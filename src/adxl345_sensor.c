#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zmk/behavior.h>
#include <zmk/hid.h>

#define MY_SERIAL DT_NODELABEL(adxl345)
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5
#define READ_INTERVAL K_SECONDS(1)  // Adjust the interval as needed

LOG_MODULE_REGISTER(adxl345);

static const struct device *sensor;

static int my_value = 42; // Example value

// Function to get the value
int get_my_value(void) {
    return my_value;
}

static int custom_key_pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", get_my_value());
    
    // Send the value as a string over HID
    zmk_hid_send_string(buffer);
    
    return ZMK_BEHAVIOR_RET_DONE;
}

static const struct behavior_driver_api custom_key_driver_api = {
    .binding_pressed = custom_key_pressed,
};

DEVICE_DEFINE(custom_key, "custom_key", NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &custom_key_driver_api);

static void adxl345_read_data(const struct device *sensor) {
    struct sensor_value accel_x, accel_y, accel_z;
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
            sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_X, &accel_x);
            sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_Y, &accel_y);
            sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_Z, &accel_z);
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
