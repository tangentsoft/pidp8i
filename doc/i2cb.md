
# I2C Bridge Device I2CB

The I2CB device is a PiDP8 specific addition to the PDP-8 simulation of SIMH.

On the Raspberry Pi, it allows to selectively offload the processing of 
IOT instruction for otherwise unimplemented device numbers to (typically) 
microcontrollers connected to the host computer via the I2C bus. 

It's use is best described in an example:


Let's say you want to interface your PDP-8 software with an **A**nalog-to-**D**igital **C**onverter, mimicking the 653X and 654X IOT instruction API used by several DEC products.

As far as SIMH is concerned, all you need to do in the boot script is:

; enable the bridge device, as it is disabled by default  
SET I2CB ENABLED  
; enable two "units" of the device, one for the  ADC  (device 53)  
; and one for a multiplexer (device 54 )    
SET I2CB0 ENABLED  
SET I2CB1 ENABLED  
; configure a PDP-8 device number to I2C device mapping,  
; here both devices are implemented with a single   
; microcontroller at address 0x11 on the I2C bus  
SET I2CB0 DEVNO=53  
SET I2CB1 DEVNO=54  
SET I2CB0 I2CADDR=11  
SET I2CB1 I2CADDR=11  

And that's it. Now you can use IOT commands in your PDP-8 code like ADSF, ADCV and ADRB to "talk" to an ADC on a
microcontroller over the I2C bus, so of course you need to implement some logic there, but with languages like
MicroPython this is relatively easy compared to, say, changing the code in SIMH to implement specific devices.



Enjoy!


### <a id="license"></a>License

Copyright © 2021 by Heinz-Bernd Eggenstein. Licensed under the terms of
[the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
