/*
    \file   main.c

    \brief  How To Use the 12-Bit Differential ADC in Series Accumulation Mode

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
 * How To Use the 12-Bit Differential ADC in Series Accumulation Mode:
 * Scaling
 *
 */

#define F_CPU 3333333ul

#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#define TIMEBASE_VALUE          ((uint8_t) ceil(F_CPU*0.000001))
#define ADC_MAX_VALUE           ((1 << 12) - 1)                 /* In single-ended mode, the max value is 4095 */
#define ADC_MAX_VALUE_16BIT     ((uint32_t) ADC_MAX_VALUE << 4)     /* Left shift 12-bit max value up to 16 bits */

/* Defines to easily configure ADC accumulation */
#define ADC_SAMPNUM_CONFIG  ADC_SAMPNUM_ACC256_gc
/* Left shifting (1 << SAMPNUM) results in the number of accumulated samples */
#define ADC_SAMPLES         (1 << ADC_SAMPNUM_CONFIG)

/* Volatile variables to improve debug experience */
static volatile uint16_t adc_reading;
static volatile float voltage;

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

	ADC0.MUXPOS = ADC_MUXPOS_VDDDIV10_gc; /* ADC channel VDD/10 */
	ADC0.COMMAND = ADC_MODE_SERIES_SCALING_gc; /* Series Accumulation with Scaling */
}

int main(void)
{
	adc_init();

	while(1)
	{
		ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;
		while(!(ADC0.INTFLAGS & ADC_SAMPRDY_bm)); /* Wait until conversion is done */
		ADC0.INTFLAGS = ADC_SAMPRDY_bm; /* Clear Sample Ready interrupt flag */

		if(ADC0.INTFLAGS & ADC_RESRDY_bm) /* If result is ready */
		{
			adc_reading = ADC0.RESULT; /* Read 16 bit scaled or left adjusted result */
			/* Calculate VDD, VREF = 1.024V, 16-bit resolution.
			    Multiplied by 10 because the input channel is VDD/10. */
			voltage = (float)(adc_reading * 1.024 * 10) / ADC_MAX_VALUE_16BIT;
		}

		_delay_ms(10);
	}
}