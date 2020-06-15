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
 * Events
 *
 */

#define F_CPU 3333333ul

#include <avr/io.h>
#include <math.h>

#define TIMEBASE_VALUE      ((uint8_t) ceil(F_CPU*0.000001))
#define ADC_MAX_VALUE       (((1 << 12) / 2) - 1) /* In differential mode, the max value is 2047 */

/* Defines to easily configure ADC accumulation */
#define ADC_SAMPNUM_CONFIG  ADC_SAMPNUM_ACC8_gc
/* Left shifting (1 << SAMPNUM) results in the number of accumulated samples */
#define ADC_SAMPLES         (1 << ADC_SAMPNUM_CONFIG)

/* Defines to easily configure RTC event frequency */
#define ADC_SAMPLING_FREQ   100     /* Hz */
#define RTC_CLOCK           32768   /* Hz */
#define RTC_PERIOD          (RTC_CLOCK / ADC_SAMPLING_FREQ)

/* Volatile variables to improve debug experience */
static volatile int32_t adc_reading;
static volatile float voltage;

/******************************************************************************
EVSYS initialization:
Channel 0:
            Event system generator: RTC Overflow
            Event system user: ADC0
Channel 1:
            Event system generator: ADC0 Result Ready
            Event system user: EVOUTB (PIN PB2)
*******************************************************************************/
void event_system_init(void)
{
	PORTB.DIRSET = PIN2_bm; /* Configure EVOUTB to output */

	EVSYS.CHANNEL0 = EVSYS_CHANNEL0_RTC_OVF_gc;     /* RTC Overflow ->  Channel 0 */
	EVSYS.USERADC0START = EVSYS_USER_CHANNEL0_gc;   /* Channel 0    ->  ADC0 Start */

	EVSYS.CHANNEL1 = EVSYS_CHANNEL1_ADC0_RES_gc;    /* ADC RESRDY   ->  Channel 1 */
	EVSYS.USEREVSYSEVOUTB = EVSYS_USER_CHANNEL1_gc; /* Channel 1    ->  EVOUTB (PB2) */
}

/*********************************************************************************
RTC initialization
**********************************************************************************/
void rtc_init(void)
{
	while(RTC.STATUS > 0);  /* Wait for all registers to be synchronized */
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm; /* Enable RTC, no prescaler */
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; /* Select 32.768 kHz internal RC oscillator */
	RTC.PER = RTC_PERIOD;
	while(RTC.STATUS > 0);  /* Wait for all registers to be synchronized */
}

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
	ADC0.MUXNEG = ADC_MUXNEG_AIN7_gc; /* ADC channel AIN7 -> PA7 */
	/* Start ADC Burst conversion on event trigger */
	ADC0.COMMAND = ADC_DIFF_bm | ADC_MODE_BURST_gc | ADC_START_EVENT_TRIGGER_gc;
}

int main(void)
{
	event_system_init();
	rtc_init();
	adc_init();

	while(1)
	{
		if(ADC0.INTFLAGS & ADC_RESRDY_bm)  /* Check if ADC sample is ready */
		{
			/* Read accumulated ADC result, clears the interrupt flag */
			adc_reading = ADC0.RESULT;
			/* Calculate voltage on ADC pin, VDD = 3.3V, 8 samples in 12-bit resolution */
			voltage = (float)(((adc_reading * 3.3) / ADC_MAX_VALUE) / ADC_SAMPLES);
		}
	}
}