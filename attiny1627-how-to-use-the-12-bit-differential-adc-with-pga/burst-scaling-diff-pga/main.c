/*
    \file   main.c

    \brief  How To Use the 12-Bit Differential ADC in Burst Accumulation Mode

    (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

/*
 *
 * How To Use the 12-Bit Differential ADC in Burst Accumulation Mode:
 * Scaling with Differential mode and PGA
 *
 */

#define F_CPU 3333333ul

#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#define TIMEBASE_VALUE              ((uint8_t) ceil(F_CPU*0.000001))
#define ADC_DIFF_MAX_VALUE          (((1 << 12) / 2) - 1)                   /* In differential mode, the max value is 2047 */
#define ADC_DIFF_MAX_VALUE_16BIT    ((uint32_t) ADC_DIFF_MAX_VALUE << 4)    /* In differential mode, the max value for a 16-bit result is 32752 */

/* Defines to easily configure ADC accumulation */
#define ADC_SAMPNUM_CONFIG  ADC_SAMPNUM_ACC256_gc
/* Left shifting (1 << SAMPNUM) results in the number of accumulated samples */
#define ADC_SAMPLES         (1 << ADC_SAMPNUM_CONFIG)

/* Volatile variables to improve debug experience */
static volatile int32_t adc_reading;
static volatile float voltage;
static volatile float current;

/*********************************************************************************
ADC initialization
**********************************************************************************/
void adc_init()
{
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.CTRLB = ADC_PRESC_DIV2_gc; /* fCLK_ADC = 3.333333/2 MHz */
	ADC0.CTRLC = ADC_REFSEL_1024MV_gc | (TIMEBASE_VALUE << ADC_TIMEBASE_gp);
	ADC0.CTRLE = 17; /* (SAMPDUR + 0.5) * fCLK_ADC = 10.5 µs sample duration */
	ADC0.CTRLF = ADC_LEFTADJ_bm | ADC_SAMPNUM_CONFIG; /* Enable left adjust if accumulating < 16 samples */

	ADC0.MUXPOS = ADC_VIA_PGA_gc | ADC_MUXPOS_AIN6_gc; /* ADC channel AIN6 -> PA6 */
	ADC0.MUXNEG = ADC_VIA_PGA_gc | ADC_MUXNEG_AIN7_gc; /* ADC channel AIN7 -> PA7 */
	ADC0.COMMAND = ADC_DIFF_bm | ADC_MODE_BURST_SCALING_gc; /* Burst Accumulation with Scaling */

	/* Enable PGA with 16x gain.
	Set full bias current for fast sampling. Configure ADCPGASAMPDUR according to data sheet. */
	ADC0.PGACTRL = ADC_GAIN_16X_gc | ADC_PGABIASSEL_1X_gc | ADC_ADCPGASAMPDUR_6CLK_gc | ADC_PGAEN_bm;
}

int main(void)
{
	adc_init();

	while(1)
	{
		ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;
		while(!(ADC0.INTFLAGS & ADC_RESRDY_bm)); /* Wait until conversion is done */

		adc_reading = ADC0.RESULT; /* Read 16 bit scaled or left adjusted result */

		/* Calculate the differential voltage, VREF = 1.024V, 16-bit resolution, 16x gain. */
		voltage = (float)((adc_reading * 1.024) / ADC_DIFF_MAX_VALUE_16BIT) / 16;
		//current = voltage / 5;   /* Uncomment this line if measuring across a 5 ohm resistor in series with the power supply */

		_delay_ms(500);
	}
}