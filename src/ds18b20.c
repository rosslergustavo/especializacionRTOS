#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ds18b20.h"

// Definir el pin GPIO al que está conectado el DS18B20
#define DS18B20_PIN 22

// Funciones para manejar la comunicación 1-Wire
void ds18b20_init();
void ds18b20_write_bit(uint bit);
uint ds18b20_read_bit();
void ds18b20_write_byte(uint8_t byte);
uint8_t ds18b20_read_byte();
int ds18b20_reset();
float ds18b20_get_temp();


void ds18b20_init() {
    gpio_init(DS18B20_PIN);
    gpio_set_dir(DS18B20_PIN, GPIO_OUT);
}

int ds18b20_reset() {
    int presence;
    gpio_set_dir(DS18B20_PIN, GPIO_OUT);
    gpio_put(DS18B20_PIN, 0);
    sleep_us(480);
    gpio_set_dir(DS18B20_PIN, GPIO_IN);
    sleep_us(70);
    presence = gpio_get(DS18B20_PIN);
    sleep_us(410);
    return presence == 0 ? 1 : 0;
}

void ds18b20_write_bit(uint bit) {
    gpio_set_dir(DS18B20_PIN, GPIO_OUT);
    gpio_put(DS18B20_PIN, 0);
    sleep_us(bit ? 1 : 60);
    gpio_put(DS18B20_PIN, 1);
    sleep_us(bit ? 60 : 1);
}

uint ds18b20_read_bit() {
    uint bit;
    gpio_set_dir(DS18B20_PIN, GPIO_OUT);
    gpio_put(DS18B20_PIN, 0);
    sleep_us(1);
    gpio_set_dir(DS18B20_PIN, GPIO_IN);
    sleep_us(14);
    bit = gpio_get(DS18B20_PIN);
    sleep_us(45);
    return bit;
}

void ds18b20_write_byte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit(byte & 1);
        byte >>= 1;
    }
}

uint8_t ds18b20_read_byte() {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte >>= 1;
        if (ds18b20_read_bit()) {
            byte |= 0x80;
        }
    }
    return byte;
}

float ds18b20_get_temp() {
    uint8_t temp_lsb, temp_msb;
    int16_t temp;

    ds18b20_init();

    if (!ds18b20_reset()) {
        printf("No DS18B20 detected\n");
        return 0.0f;
    }

    ds18b20_write_byte(0xCC); // Skip ROM
    ds18b20_write_byte(0x44); // Start temperature conversion

    sleep_ms(750); // Wait for conversion to complete

    if (!ds18b20_reset()) {
        printf("No DS18B20 detected\n");
        return 0.0f;
    }

    ds18b20_write_byte(0xCC); // Skip ROM
    ds18b20_write_byte(0xBE); // Read scratchpad

    temp_lsb = ds18b20_read_byte();
    temp_msb = ds18b20_read_byte();
    temp = (temp_msb << 8) | temp_lsb;

    return temp / 16.0;
}