#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
 
static int addr = 0x68;
 
static void mpu6050_reset() {

    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(i2c0, addr, buf, 2, false);
    sleep_ms(100); 
 
    
    buf[1] = 0x00;  
    i2c_write_blocking(i2c0, addr, buf, 2, false); 
    sleep_ms(10); 
 }
 
static void mpu6050_read_raw(int16_t accel[3], int16_t gyro_raw[3], int16_t *temp) {
 
    uint8_t buffer[6];
 
    
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c0, addr, &val, 1, true);
    i2c_read_blocking(i2c0, addr, buffer, 6, false);
 
    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
 
    val = 0x43;
    i2c_write_blocking(i2c0, addr, &val, 1, true);
    i2c_read_blocking(i2c0, addr, buffer, 6, false); 
 
    for (int i = 0; i < 3; i++) {
        gyro_raw[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }
 
    val = 0x41;
    i2c_write_blocking(i2c0, addr, &val, 1, true);
    i2c_read_blocking(i2c0, addr, buffer, 2, false); 
 
    *temp = buffer[0] << 8 | buffer[1];
 }

int main() {

    stdio_init_all();
    float accel_gravity = 16384.f;
    float gyro_constant = 131.07f;

    printf("Hello, MPU6050! Reading raw data from registers...\n");
 
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    bi_decl(bi_2pins_with_func(0, 1, GPIO_FUNC_I2C));
 
    mpu6050_reset();
 
    int16_t acceleration[3], gyro_raw[3], temp;
    float acceleration_g[3], gyro[3];
 
    while (1) {

        mpu6050_read_raw(acceleration, gyro_raw, &temp);

        for (int i = 0; i < 3; i++) {
            acceleration_g[i] = ((acceleration[i])/accel_gravity);
        }

        for (int i = 0; i < 3; i++) {
            gyro[i] = ((gyro_raw[i])/gyro_constant);
        }

        //printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Acc. X = %.2f g, Y = %.2f g, Z = %.2f g\n", acceleration_g[0], acceleration_g[1],(acceleration_g[2] - 0.15f));
        //printf("gyro_raw. X = %d, Y = %d, Z = %d\n", gyro_raw[0], gyro_raw[1], gyro_raw[2]);
        printf("gyro X = %.2f °/s, Y = %.2f °/s, Z = %.2f °/s\n", gyro[0], gyro[1], gyro[2]);
 
        sleep_ms(500);
    }
}