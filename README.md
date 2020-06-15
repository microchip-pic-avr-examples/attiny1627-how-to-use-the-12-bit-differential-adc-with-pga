<a href="https://www.microchip.com" rel="nofollow"><img src="images/microchip.png" alt="MCHP" width="300"/></a>

# How to Use the 12-Bit Differential ADC with PGA

These code examples show how to use the different modes available when using the 12-bit differential ADC with PGA of the ATtiny1627 microcontroller. The features are explained further in the accompanying tech briefs.

## Related Documentation
- Technical Briefs:
  - [TB3256 - How to Use the 12-Bit Differential ADC with PGA in Single Mode](https://microchip.com/DS90003256)
  - [TB3257 - How to Use the 12-Bit Differential ADC with PGA in Series Accumulation Mode](https://microchip.com/DS90003257)
  - [TB3254 - How to Use the 12-Bit Differential ADC with PGA in Burst Accumulation Mode](https://microchip.com/DS90003254)
- Data Sheets available on Product Pages:
  - [ATtiny1624](https://microchip.com/wwwproducts/en/ATtiny1624)
  - [ATtiny1626](https://microchip.com/wwwproducts/en/ATtiny1626)
  - [ATtiny1627](https://microchip.com/wwwproducts/en/ATtiny1627)



## Software Used
- [Atmel Studio](https://www.microchip.com/mplab/avr-support/atmel-studio-7) 7.0.2397 or later
- Atmel Studio ATtiny_DFP version 1.4.310 or later




## Hardware Used
- [ATtiny1627](https://microchip.com/wwwproducts/en/ATtiny1627)



## Setup and Operation
<!-- Explain how to connect hardware and set up software. Depending on complexity, step-by-step instructions and/or tables and/or images can be used -->

***
<b>Single Mode</b>
- <b>Window Comparator:</b>
  - Location:
      - Atmel Studio project name: `single-window-comparator`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-window-comparator`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-window-comparator)
  - Setup:
      - ADC input: AIN6 -> PA6
  - Description: 
      - This code example shows how the Window Comparator may be used to filter out spikes in an analog signal. This is done by acting on the ADC result in the Window Comparator interrupt handler, thereby ignoring the results that are outside of the configured window.
  - Instructions:
      - Connect a signal to PA6. The signal must range between GND and V<sub>DD</sub>. When the signal is within ~0.5 \*V<sub>DD</sub> and ~0.73 \* V<sub>DD</sub>, the window compare interrupt is triggered, and the voltage is calculated. Note that the voltage calculation is based on V<sub>DD</sub> being 3.3V. For other V<sub>DD</sub>, please change the voltage calculation in the ISR accordingly.

- <b>Event Trigger:</b>
  - Location:
      - Atmel Studio project name: `single-event-trigger`
      - Path:  [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-event-trigger`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-event-trigger)
  - Setup:
      - Positive ADC input: AIN6 -> PA6
      - Negative ADC input: AIN7 -> PA7
  - Description:
      - This code example shows how to trigger the ADC using triggers from the event system. In this case, the RTC provides the events used to trigger the ADC. The ADC measurements are differential, measuring the difference in voltage between PA6 and PA7.
  - Instructions:
      - Connect signals to PA6 and PA7. Both signals must range between GND and V<sub>DD</sub>. The ADC will trigger once every 10 ms. Note that the voltage calculation is based on V<sub>DD</sub> being 3.3V. For other V<sub>DD</sub>, please change the voltage calculation in the `main()` function accordingly.

- <b>Measuring V<sub>DD</sub>:</b>
  - Location:
      - Atmel Studio project name: `single-measuring-vdd`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-measuring-vdd`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-measuring-vdd)
  - Setup:
      - V<sub>DD</sub> is an internal signal, so no external signal source is needed
  - Description:
      - This code example shows how to measure and interpret the V<sub>DD</sub> supplied to the microcontroller using the ADC in 12-bit mode. The results are transmitted using USART, and can be interpreted by the Data Visualizer. 
  - Instructions:
      - To see the results in Data Visualizer, stream the output of PB2 via a CDC virtual COM port to the computer. This may be achieved for example using a [Curiosity Nano](https://www.microchip.com/developmenttools/ProductDetails/DM080104) board or the [Power Debugger](https://www.microchip.com/developmenttools/ProductDetails/ATPOWERDEBUGGER).
  
- <b>Measuring Temperature:</b>
  - Location:
      - Atmel Studio project name: `single-measuring-temp`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-measuring-temp`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/single-measuring-temp)
  - Setup: 
      - The temperature sensor is internal, so no external signal source is needed
  - Description:
      - This code example shows how to measure and interpret the internal temperature sensor using the ADC in 12-bit mode. The code calculates the temperature in kelvin and celcius.
  - Instructions:
      - The results of the temperature measurement can be seen by placing a breakpoint in the `while(1)` loop in the `main()` function, and using a debugger to start a debug session. When the device is halted, the variables that are interesting may be placed in the watch list to see their values.
 ***
 
<b>Series Accumulation Mode</b>
- <b>Window Comparator:</b>
  - Location:
      - Atmel Studio project name: `series-window-comparator`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-window-comparator`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-window-comparator)
  - Setup:
      - ADC input: AIN6 -> PA6
  - Description:
      - This code example shows how the Window Comparator may be used to filter out spikes in an analog signal. This is done by restarting the series accumulation in the window comparator interrupt handler. When the result is ready, the result is acted on.
  - Instructions:
      - Connect a signal to PA6. The signal must range between GND and V<sub>DD</sub>. When the signal is below ~0.5 \*V<sub>DD</sub> or above ~0.73 \* V<sub>DD</sub>, the window compare interrupt is not triggered, and the voltage is calculated when all the samples have been converted. Note that the voltage calculation is based on V<sub>DD</sub> being 3.3V. For other V<sub>DD</sub>, please change the voltage calculation in the ISR accordingly.

- <b>Event Trigger:</b>
  - Location:
      - Atmel Studio project name: `series-event-trigger`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-event-trigger`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-event-trigger)
  - Setup:
      - Positive ADC input: AIN6 -> PA6
      - Negative ADC input: AIN7 -> PA7
  - Description:
      - This code example shows how to trigger the ADC using triggers from the event system. In this case, the RTC provides the events used to trigger the ADC. The ADC measurements are differential, measuring the difference in voltage between PA6 and PA7.
  - Instructions:
      - Connect signals to PA6 and PA7. Both signals must range between GND and V<sub>DD</sub>. The ADC will trigger once every 10 ms. Note that the voltage calculation is based on V<sub>DD</sub> being 3.3V. For other V<sub>DD</sub>, please change the voltage calculation in the `main()` function accordingly.
  
- <b>Oversampling:</b>
  - Location:
      - Atmel Studio project name: `series-oversampling`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-oversampling`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-oversampling)
  - Setup:
      - ADC input: AIN6 -> PA6
  - Description:
      - This code example shows how to use oversampling to increase resolution from 12 to 17 bits. The ADC samples the signal once per ~1 ms, and the samples are automatically accumulated into the result register when 1024 samples have been converted.
  - Instructions:
      - Connect a signal to PA6. The signal must range between GND and V<sub>DD</sub>. To see the 17-bit result, place a breakpoint in the `while(1)` loop in the `main()` function and use a debugger to start a debug session. When the device is halted, the variables that are interesting may be placed in the watch list to see their values.

- <b>Scaling:</b>
  - Location:
      - Atmel Studio project name: `series-scaling`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-scaling`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/series-scaling)
  - Setup: 
      - The V<sub>DD</sub> is used as the ADC input, so no external signal source is needed
  - Description:
      - This code example shows how to measure V<sub>DD</sub> using the Series Accumulation with Scaling mode to automatically scale the result after having accumulated multiple samples. The example also uses oversampling to achieve 16-bit resolution.
  - Instructions:
      - To see the 16-bit V<sub>DD</sub> measurements, place a breakpoint in the `while(1)` loop in the `main()` function and use a debugger to start a debug session. When the device is halted, the variables that are interesting may be placed in the watch list to see their values.
***
<b>Burst Accumulation Mode</b>
- <b>Window Comparator:</b>
  - Location:
      - Atmel Studio project name: `burst-window-comparator`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-window-comparator`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-window-comparator)
  - Setup:
      - ADC input: AIN6 -> PA6
  - Description:
      - This code example shows how the Window Comparator may be used to filter out spikes in an analog signal. This is done by restarting the burst accumulation in the window comparator interrupt handler. When the result is ready, the result is acted on.
  - Instructions:
      - Connect a signal to PA6. The signal must range between GND and V<sub>DD</sub>. When the signal is below ~0.5 \*V<sub>DD</sub> or above ~0.73 \* V<sub>DD</sub>, the window compare interrupt is not triggered, and the voltage is calculated when all the samples have been converted. Note that the voltage calculation is based on V<sub>DD</sub> being 3.3V. For other V<sub>DD</sub>, please change the voltage calculation in the ISR accordingly.

- <b>Event Trigger:</b>
  - Location:
      - Atmel Studio project name: `burst-event-trigger`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-event-trigger`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-event-trigger)
  - Setup:
      - Positive ADC input: AIN6 -> PA6
      - Negative ADC input: AIN7 -> PA7
  - Description:
      - This code example shows how to trigger the ADC using triggers from the event system. In this case, the RTC provides the events used to trigger the ADC. The ADC measurements are differential, measuring the difference in voltage between PA6 and PA7.
  - Instructions:
      - Connect signals to PA6 and PA7. Both signals must range between GND and V<sub>DD</sub>. The ADC will trigger once every 10 ms. Note that the voltage calculation is based on V<sub>DD</sub> being 3.3V. For other V<sub>DD</sub>, please change the voltage calculation in the `main()` function accordingly.
  
- <b>Oversampling:</b>
  - Location:
      - Atmel Studio project name: `burst-oversampling`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-oversampling`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-oversampling)
  - Setup:
      - ADC input: AIN6 -> PA6
  - Description:
      - This code example shows how to use oversampling to increase resolution from 12 to 17 bits. The ADC samples the signal as fast as possible, and the samples are automatically accumulated into the result register when 1024 samples have been converted.
  - Instructions:
      - Connect a signal to PA6. The signal must range between GND and V<sub>DD</sub>. To see the 17-bit result, place a breakpoint in the `while(1)` loop in the `main()` function and use a debugger to start a debug session. When the device is halted, the variables that are interesting may be placed in the watch list to see their values.

- <b>Scaling with Programmable Gain Amplifier (PGA):</b>
  - Location:
      - Atmel Studio project name: `burst-scaling-diff-pga`
      - Path: [`./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-scaling-diff-pga`](./attiny1627-how-to-use-the-12-bit-differential-adc-with-pga/burst-scaling-diff-pga)
  - Setup:
      - Positive ADC input: AIN6 -> PA6
      - Negative ADC input: AIN7 -> PA7
  - Description:
      - This code example shows how to perform differential measurements using the Burst Accumulation with Scaling mode, the PGA with 16x gain and oversampling to achieve 16-bit resolution. 
  - Instructions:
      - Connect signals to PA6 and PA7. The difference between the signals must range between 0V and 64 mV, and the signals must range between GND and V<sub>DD</sub>. To see the 16-bit result, place a breakpoint in the `while(1)` loop in the `main()` function and use a debugger to start a debug session. When the device is halted, the variables that are interesting may be placed in the watch list to see their values. If measuring across a 5 ohm resistor, the second to last line in the `main()` function can be uncommented to measure the current through the resistor. This is further explained in the corresponding technical brief.
***

## Conclusion

The examples have shown how to use the 12-bit differential ADC with PGA in its different operating modes and combinations thereof.
