/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 #include "hardware/timer.h"
 #include "hardware/adc.h"
 
 const int PIN_LED_B = 4;
 
 const float conversion_factor = 3.3f / (1 << 12);

 volatile int flag_timer = 0;

 bool timer_callback(repeating_timer_t *rt) {
     flag_timer = 1;
     return true;
 }
 
 /**
  * 0..1.0V: Desligado
  * 1..2.0V: 150 ms
  * 2..3.3V: 400 ms
  */
 int main() {
     stdio_init_all();

     gpio_init(PIN_LED_B);
     gpio_set_dir(PIN_LED_B, GPIO_OUT);
     gpio_put(PIN_LED_B, 0);

     adc_init();
     adc_gpio_init(28);
     adc_select_input(2);

     repeating_timer_t timer;
     int periodo_atual = 0;
     int led = 0;

     while (1) {
         uint16_t result = adc_read();
         float voltage = result * conversion_factor;

         int periodo;
         if (voltage < 1.0) {
             periodo = 0;
         } else if (voltage < 2.0) {
             periodo = 300;
         } else {
             periodo = 500;
         }

         if (periodo != periodo_atual) {
             if (periodo_atual > 0) {
                 cancel_repeating_timer(&timer);
             }

             if (periodo > 0) {
                 add_repeating_timer_ms(periodo, timer_callback, NULL, &timer);
             } else {
                 flag_timer = 0;
                 led = 0;
                 gpio_put(PIN_LED_B, 0);
             }

             periodo_atual = periodo;
         }

         if (flag_timer) {
             flag_timer = 0;
             led = !led;
             gpio_put(PIN_LED_B, led);
         }
     }
 }