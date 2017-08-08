/*  Copyright (c) 2016, Appiko
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of its contributors
 *  may be used to endorse or promote products derived from this software without
 *  specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 *  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @addtogroup group_appln
 * @{
 *
 * @defgroup bluey_demo An application to demo the Bluey's capabilities
 * @brief A Bluey demo
 * @{
 */

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"

#include "boards.h"
#include "hal_clocks.h"
#include "hal_nop_delay.h"
#include "hal_gpio.h"
#include "common_util.h"
#include "tinyprintf.h"
#include "uart_printf.h"
#include "us_timer.h"
#include "nrf_util.h"
#include "ble_adv.h"
#include "ms_timer.h"
#include "simple_adc.h"

int16_t noise_level = 0;
int16_t co2_level = 0;

/** @brief Configure the RGB LED pins as output and turn off LED */
static void rgb_led_init(void)
{
    hal_gpio_cfg_output(LED_RED, !(LEDS_ACTIVE_STATE));
    hal_gpio_cfg_output(LED_GREEN, !(LEDS_ACTIVE_STATE));
    hal_gpio_cfg_output(LED_BLUE, !(LEDS_ACTIVE_STATE));
}

/** @brief Configure the RGB LED pins as output and turn off LED */
static void rgb_led_cycle(void)
{
    hal_gpio_pin_write(LED_RED, (LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_GREEN, !(LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_BLUE, !(LEDS_ACTIVE_STATE));
    hal_nop_delay_ms(250);
    hal_gpio_pin_write(LED_RED, !(LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_GREEN, (LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_BLUE, !(LEDS_ACTIVE_STATE));
    hal_nop_delay_ms(250);
    hal_gpio_pin_write(LED_RED, !(LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_GREEN, !(LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_BLUE, (LEDS_ACTIVE_STATE));
    hal_nop_delay_ms(250);
    hal_gpio_pin_write(LED_RED, !(LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_GREEN, !(LEDS_ACTIVE_STATE));
    hal_gpio_pin_write(LED_BLUE, !(LEDS_ACTIVE_STATE));
}
/** @brief turn off Red LED */
static void led_red_off(void)
{
    hal_gpio_pin_write(LED_RED, !(LEDS_ACTIVE_STATE));
}
/** @brief turn on Red LED */
static void led_red_on(void)
{
    hal_gpio_pin_write(LED_RED, (LEDS_ACTIVE_STATE));
}

/** @brief read_adc pin 1 and 2*/
static void Read_adc(void)
{
    static int32_t adc_val=0,adc_val2=0;
    static int16_t count =0;

    if(count==20)
    {
      adc_val=adc_val/20;
      adc_val2=adc_val2/20;
      tfp_printf("Noise  level is %ld,smoke level is %ldppm\n", adc_val,adc_val2);
      count=0;
      adc_val=0;
    }

    led_red_on();
    hal_nop_delay_ms(10);
    led_red_off();

    adc_val+= simple_adc_get_value(SIMPLE_ADC_GAIN1_6, ANALOG_PIN_2);
    adc_val2+= simple_adc_get_value(SIMPLE_ADC_GAIN1_6, ANALOG_PIN_3);
    //pnarasim : tbd
    // set the noise and co2 value here in the global vars
    noise_level = adc_val;
    co2_level = adc_val2;
count++;
 }
// create ble beacon
static void create_noise_adv(void){
    dump_log();
    static uint8_t count = 0;
    count++;
    
    uint8_t noise_adv_data[] = {
            /* Len, type, data */
            0x07, GAP_ADV_NAME_FULL, 'H', 'W','-', 'I', 'D', '2',
	    0x02, HW_GAP_ADV_NOISE_LEVEL, noise_level, 
	    0x02, HW_GAP_ADV_CO2_LEVEL, co2_level, 
            0x02, 0xFF,0x004C
	};
    ble_adv_set_adv_data(sizeof(noise_adv_data), noise_adv_data);
}

#if 0
static void log_dump_handler(void){
    dump_log();
    static uint8_t count = 0;
    count++;
    uint8_t adv_data[] = {
            /* Len, type, data */
            0x02, GAP_ADV_FLAGS, 0x06,
            0x06, GAP_ADV_NAME_FULL, 'N', 'o', 'i', 's', 'e',
            0x02, 0xFF, 0x004C,
	    0x11, GAP_ADV_UUID128_ALL, 0x09ea3550b2924c0586f1ba5cf9202894,
	    0x04, GAP_ADV_SERVICE_DATA, 'n', 'o', 'v', 'a',
	};
    ble_adv_set_adv_data(sizeof(adv_data), adv_data);
}
#endif

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    void (*adc_check)();
    //set up function pointers
    adc_check = &Read_adc;
    rgb_led_init();
    rgb_led_cycle();
    /* Initial printf */
    uart_printf_init(UART_PRINTF_BAUD_115200);
    tfp_printf("Hello World %d!\n", 42);

    ms_timer_init(APP_IRQ_PRIORITY_LOWEST);
    us_timer_init(APP_IRQ_PRIORITY_LOWEST);

    hfclk_xtal_init_blocking();
    lfclk_init(LFCLK_SRC_Xtal);

    uint8_t adrs[] = {0x0B, 0x0E, 0x0A, 0x0C, 0x00, 0x01};
    uint8_t adv_data[] = {
            /* Len, type, data */
            0x02, GAP_ADV_FLAGS, 0x06,
            0x07, GAP_ADV_NAME_FULL, 'H', 'W', '-', 'I', 'D', '2',
            0x02, 0xFF, 0x4c,
    };

    uint8_t scan_rsp[] = {
            0x02, GAP_ADV_TRANSMIT_PWR, 0
    };

    ble_adv_param_t param = {RTC_TICKS_625(100), ADV_SCAN_IND_PARAM, RANDOM_ADRS_PARAM, CH_ALL_PARAM};

    ble_adv_set_random_adrs(adrs);
    ble_adv_set_tx_power(0);
    ble_adv_set_adv_data(sizeof(adv_data), adv_data);
    ble_adv_set_adv_param(&param);
    ble_adv_set_scan_rsp_data(sizeof(scan_rsp), scan_rsp);

    ble_adv_start();

    //ms_timer_start(MS_TIMER0, MS_REPEATED_CALL, RTC_TICKS_MS(1027), log_dump_handler);
    ms_timer_start(MS_TIMER0, MS_REPEATED_CALL, RTC_TICKS_MS(1027), create_noise_adv);
    ms_timer_start(MS_TIMER1, MS_REPEATED_CALL, RTC_TICKS_MS(100), adc_check);

    tfp_printf("While %d!\n", 1);

    while (true)
    {
        __WFI();
    }
}

/** @} */
/** @} */
