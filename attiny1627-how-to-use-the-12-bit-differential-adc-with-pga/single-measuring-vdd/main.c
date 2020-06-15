/*
    \file   main.c

    \brief  How To Use the 12-Bit Differential ADC in Single Mode

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
 * How To Use the 12-Bit Differential ADC in Single Mode:
 * Measuring VDD
 *
 */

#define F_CPU 3333333ul

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#define TIMEBASE_VALUE ((uint8_t) ceil(F_CPU*0.000001))
#define ADC_MAX_VALUE   ((1 << 12) - 1) /* In single-ended mode, the max value is 4095 */
#define BAUD_RATE 9600
#define BAUD_REG_VAL ((float)(64 * F_CPU / (16 * (float)BAUD_RATE)) + 0.5)

static uint16_t adc_reading;
static float voltage;

/**********************************************************************************
ADC initialization
**********************************************************************************/
void adc_init()
{
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.CTRLB = ADC_PRESC_DIV2_gc; /* fCLK_ADC = 3.333333/2 MHz */
	ADC0.CTRLC = ADC_REFSEL_1024MV_gc | (TIMEBASE_VALUE << ADC_TIMEBASE_gp);
	ADC0.CTRLE = 17; /* (SAMPDUR + 0.5) * fCLK_ADC = 10.5 µs sample duration */

	ADC0.MUXPOS = ADC_MUXPOS_VDDDIV10_gc; /* ADC channel VDD/10 */
	ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc; /* Single 12-bit mode */
}

/**********************************************************************************
USART initialization
**********************************************************************************/
void usart_init()
{
	PORTB.DIRSET = PIN2_bm; /* Set TXD to output */
	USART0.CTRLB = USART_TXEN_bm; /* Enable USART transmitter */
	USART0.BAUD = BAUD_REG_VAL; /* Set baud rate */
}

/**********************************************************************************
Send float via USART to Data Visualizer
**********************************************************************************/
void USART_send_DV(float *float_ptr)
{
	uint8_t *byte_ptr = (uint8_t *) float_ptr;

	while(!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = 0x33; /* Send data stream start byte */

	for(uint8_t i = 0; i < sizeof(float); i++)
	{
		while(!(USART0.STATUS & USART_DREIF_bm));
		USART0.TXDATAL = byte_ptr[i];
	}

	while(!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = ~0x33; /* Send data stream stop byte */
}

int main(void)
{
	adc_init();
	usart_init();

	while(1)
	{
		ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;     /* Start ADC conversion */
		while(!(ADC0.INTFLAGS & ADC_SAMPRDY_bm));   /* Wait until conversion is done */

		adc_reading = ADC0.SAMPLE; /* Read ADC sample, clears flag */
		/* Calculate VDD, VREF = 1.024V, 12-bit resolution.
		   Multiplied by 10 because the input channel is VDD/10. */
		voltage = (float)(adc_reading * 1.024 * 10) / ADC_MAX_VALUE;

		USART_send_DV(&voltage); /* Transmit voltage to Data Visualizer */

		_delay_ms(500);
	}
}
