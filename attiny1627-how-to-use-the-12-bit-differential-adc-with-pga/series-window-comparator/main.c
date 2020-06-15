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
 * Window Comparator
 *
 */

#define F_CPU 3333333ul

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#define TIMEBASE_VALUE      ((uint8_t) ceil(F_CPU*0.000001))
#define ADC_MAX_VALUE       ((1 << 12) - 1) /* In single-ended mode, the max value is 4095 */

/* Defines to easily configure ADC accumulation */
#define ADC_SAMPNUM_CONFIG  ADC_SAMPNUM_ACC256_gc
/* Left shifting (1 << SAMPNUM) results in the number of accumulated samples */
#define ADC_SAMPLES         (1 << ADC_SAMPNUM_CONFIG)

/* Volatile variables to improve debug experience */
static volatile uint32_t adc_reading;
static volatile float voltage;

/**********************************************************************************
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

	ADC0.WINHT = 3000; /* Window High Threshold */
	ADC0.WINLT = 2000; /* Window Low Threshold */
	/* Window Comparator mode: Outside. Use SAMPLE register as Window Comparator source */
	ADC0.CTRLD = ADC_WINCM_OUTSIDE_gc | ADC_WINSRC_SAMPLE_gc;
	/* Enable Window Compare and Result Ready interrupt */
	ADC0.INTCTRL = ADC_WCMP_bm | ADC_RESRDY_bm;

	ADC0.COMMAND = ADC_MODE_SERIES_gc; /* Series Accumulation mode */
}

/***********************************************************************************
Window Compare interrupt:
In this example, when a sample is outside a certain window, this is considered an
invalid signal spike. When such a spike is detected, the accumulated ADC result is
disregarded by restarting the series conversion.
***********************************************************************************/
ISR(ADC0_SAMPRDY_vect)
{
	ADC0.INTFLAGS = ADC_WCMP_bm;        /* Clear WCMP flag */

	/* Clear the accumulator by resetting the Mode bit field */
	ADC0.COMMAND = ADC_START_STOP_gc;
	/* Reconfigure Series Accumulation mode */
	ADC0.COMMAND = ADC_MODE_SERIES_gc;
}

/***********************************************************************************
Result Ready interrupt:
If no spike was detected, the result is read and the corresponding
voltage is calculated
***********************************************************************************/
ISR(ADC0_RESRDY_vect)
{
	ADC0.INTFLAGS = ADC_RESRDY_bm;      /* Clear RESRDY flag */

	/* Check if the last sample was inside the window */
	if(!(ADC0.INTFLAGS & ADC_WCMP_bm))
	{
		adc_reading = ADC0.RESULT; /* Read ADC result */
		/* Calculate voltage on ADC pin, VDD = 3.3V, 12-bit resolution, 256 samples */
		voltage = (float)(((adc_reading * 3.3) / ADC_MAX_VALUE) / ADC_SAMPLES);
	}
}

int main(void)
{
	adc_init();
	sei(); /* Enable global interrupts */

	while(1)
	{
		/* Start a conversion once every 1 ms */
		ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;
		_delay_ms(1);
	}
}