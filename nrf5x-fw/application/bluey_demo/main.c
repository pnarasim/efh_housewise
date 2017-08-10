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
#include "math.h"

int16_t noise_level = 0;
int16_t co2_level = 0;

#define BYTE_FACTOR 16
//Ro is initialized to 10 kilo ohms
float Ro = 10; 

//define the load resistance on the board, in kilo ohms
#define RL_VALUE (10)
//(Sensor resistance in clean air)/RO which is derived from the chart in datasheet
#define RO_CLEAN_AIR_FACTOR (9.83)
#define SMOKE (1)

//two points from LPG curve are taken point1:(200,1.6) point2(10000,0.26) 
//take log of each point (lg200, lg 1.6)=(2.3,0.20)  (lg10000,lg0.26)=(4,-0.58) 
//find the slope using these points. take point1 as reference
float LPGCurve[3] = {2.3,0.20,-0.45};
                                                                                                          
//data format:{ x, y, slope};
//two points from smoke curve are taken point1:(200,3.4) point2(10000,0.62) 
//take log of each point (lg200, lg3.4)=(2.3,0.53)  (lg10000,lg0.63)=(4,-0.20) 
//find the slope using these points. take point1 as reference
float SmokeCurve[3] ={2.3,0.53,-0.43};
                                                      

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

/** @brief read_adc pin 1 and 2 every 100ms but averages for 2 sec*/
static int GetPercentage(float ratio, float *curve)
{                                                                          
  //Using slope,ratio(y2) and another point(x1,y1) on line we will find
  // gas concentration(x2) using x2 = [((y2-y1)/slope)+x1]
  // as in curves are on logarithmic coordinate, power of 10 is taken to convert result to non-logarithmic.
  return (pow(10,( ((log(ratio)-curve[1])/curve[2]) + curve[0])));
}


float ResistanceCalculation(uint32_t raw_adc)
{                                                         
  // sensor and load resistor forms a voltage divider. so using analog value and load value
  // we will find sensor resistor.
  return ( ((float)RL_VALUE*(4096-raw_adc)/raw_adc));     
}

static void Read_adc(void)
{
    static float adc_val=0;
    static int16_t adc_val2=0;
    static int16_t count =0;
    int16_t ppm=0;


    if(count==20)
    {
      adc_val=adc_val/20;
      adc_val = adc_val/Ro;

      adc_val2 = adc_val2/20;
      ppm=GetPercentage(adc_val, SmokeCurve);
      tfp_printf("Noise  level is %d,smoke level is %d ppm\n", adc_val2,ppm);
      // set the noise and co2 value here in the global vars
      noise_level =adc_val2;
      co2_level = ppm;
      //tfp_printf("noise=0x%x co2=0x%x\n", noise_level, co2_level);

      count=0;
      adc_val=0;
      adc_val2=0;
    }

    led_red_on();
    hal_nop_delay_ms(10);
    led_red_off();

    adc_val+= ResistanceCalculation(simple_adc_get_value(SIMPLE_ADC_GAIN1_6, ANALOG_PIN_2));

    adc_val2+= simple_adc_get_value(SIMPLE_ADC_GAIN1_6, ANALOG_PIN_3);

    count++;
 }



// This function assumes that sensor is in clean air.
 float SensorCalibration()
{
  int i;                                   
  float val=0;

  //take multiple samples and calculate the average value
  for (i=0;i<50;i++) {                   
    val += ResistanceCalculation(simple_adc_get_value(SIMPLE_ADC_GAIN1_6, ANALOG_PIN_2));
    hal_nop_delay_ms(100);
  }
  val = val/50;
  //divided by RO_CLEAN_AIR_FACTOR yields the Ro
  //according to the chart in the datasheet
  val = val/RO_CLEAN_AIR_FACTOR;                        
                                                    
  return val;
}

// create ble beacon
static void create_noise_adv(void){
    dump_log();
    static uint8_t beacon_count = 0;
    beacon_count++;
    int8_t n=0, c=0;

    n=noise_level/BYTE_FACTOR;
    c=co2_level/BYTE_FACTOR;
    tfp_printf("noise=0x%x co2=0x%x n=0x%x, c=0x%x\n", noise_level, co2_level, n, c);
    
    uint8_t noise_adv_data[] = {
            /* Len, type, data */
	    0x04, GAP_ADV_NAME_FULL, 'H', 'W', '2',
	    0x02, HW_GAP_ADV_NOISE_LEVEL, n, 
	    0x02, HW_GAP_ADV_CO2_LEVEL, c,     
	    0x03, GAP_ADV_MANUF_DATA, 0x004C
	};
    tfp_printf("Size of ble adv data = %d\n", sizeof(noise_adv_data));
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
    tfp_printf("Calibrating Smoke sensor");
    Ro=SensorCalibration(); //Please make sure the sensor is in clean air
    //when you perform the calibration
    tfp_printf("Calibration is done...\n\r");
    tfp_printf("Ro=%4dkohm ",(int16_t)Ro);
    tfp_printf("\n\r");
    tfp_printf("sizeof float = %d\n", sizeof(float));
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
