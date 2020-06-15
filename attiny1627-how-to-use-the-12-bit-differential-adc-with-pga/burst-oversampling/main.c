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
 * Oversampling
 *
 */

#define F_CPU 3333333ul

#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#define TIMEBASE_VALUE          ((uint8_t) ceil(F_CPU*0.000001))

/* Defines to configure ADC accumulation */
#define OVERSAMPLING_BITS       5 /* 5 bits extra */
#define OVERSAMPLING_MAX_VALUE  ((uint32_t) ((1 << 12) - 1) << OVERSAMPLING_BITS) /* 12 + 5 bits = 17 bits */
#define ADC_SAMPNUM_CONFIG      (OVERSAMPLING_BITS << 1) /* The SAMPNUM bit field setting match this formula */
#define ADC_SAMPLES             (1 << ADC_SAMPNUM_CONFIG) /* 5 bits = 1024 samples */

/* Volatile variables to improve debug experience */
static volatile uint32_t adc_reading;
static volatile float voltage;

/*********************************************************************************
ADC initialization
**********************************************************************************/
void adc_init()
{
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.CTRLB = ADC_PRESC_DIV2_gc; /* fCLK_ADC = 3.333333/2 MHz */
	ADC0.CTRLC = ADC_REFSEL_VDD_gc | (TIMEBASE_VALUE << ADC_TIMEBASE_gp);
	ADC0.CTRLE = 17; /* (SAMPDUR + 0.5) * fCLK_ADC = 10.5 µs sample duration */
	ADC0.CTRLF = ADC_SAMPNUM_CONFIG;

	ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc; /* ADC channel AIN6 -> PA6 */
	ADC0.COMMAND = ADC_MODE_BURST_gc; /* Burst Accumulation mode */
}

int main(void)
{
	adc_init();

	while(1)
	{
		ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;
		while(!(ADC0.INTFLAGS & ADC_RESRDY_bm)); /* Wait until conversion is done */

		/* Oversampling compensation as explained in the tech brief */
		adc_reading = ADC0.RESULT >> OVERSAMPLING_BITS; /* Scale accumulated result by right shifting the number of extra bits */
		voltage = (float)((adc_reading * 3.3) / OVERSAMPLING_MAX_VALUE); /* Calculate voltage using 17-bit resolution, VDD = 3.3V */
	}
}