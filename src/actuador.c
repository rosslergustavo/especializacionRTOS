#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "actuador.h"

#define RESISTENCIA_PIN 16



void resistencia_init() {
    gpio_init(RESISTENCIA_PIN);
    gpio_set_dir(RESISTENCIA_PIN, GPIO_OUT);
}

void resistencia_on()
{
    printf("Resistencia ON \n");
    gpio_put(RESISTENCIA_PIN, 1);
}

void resistencia_off()
{
    printf("Resistencia OFF \n");
    gpio_put(RESISTENCIA_PIN, 0);
    
}