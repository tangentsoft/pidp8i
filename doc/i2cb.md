
# I2C Bridge Device I2CB

**Note:** The term "IOT" used in the following refer to the IOT instruction of the PDP-8 (and not to, say 
Internet of Things, IoT)

The I2CB device (short for **I2C** **B**ridge) is a PiDP8i specific addition to the PDP-8 simulation of SIMH.

On the Raspberry Pi, it allows to selectively offload the processing of 
IOT instruction for otherwise unused IOT device numbers to (typically) 
microcontrollers connected to the host computer via the I2C bus. 
So instead of providing code by extending SIMH to deal with the processing of IOT instructions for
previously unsupported devices, the I2CB SIMH device will only forward the IOT instruction
and the value of the accumulator register to a configurable hardware device on the I2C bus, which
will then have to send a response back. This response is then received by the I2CB device and used to 
emulate the IOT instruction.


## Dependencies and build instructions

To build PiDP-8i with I2CB suppoprt, the I2CB feature requires the **libi2cd** library which can be
obtained from

https://github.com/sstallion/libi2cd

and needs to be installed to /usr/local/lib on the Raspberry PI (the default target
at the time of writing).


When building the PiDP8i software from source, this experimental feature is 
disabled by default, you need to enable it via 

./configure --i2c-bridge

and then build the software as usual.

## How to use the I2CB device 

The use of the I2CB device is best described in an example:

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
microcontroller over the I2C bus, but of course you need to implement some logic there, but with languages like
MicroPython this is relatively easy compared to, say, changing the code in SIMH to implement specific devices.

A (hopefully) growing collection of example code for the microcontroller side can be found 
here: https://github.com/Bikeman/PiDP8-I2CB-demo-examples

Enjoy!

## Technical Annex

Note: We use "Controller/Responder" nomenclature per this article: https://www.eetimes.com/its-time-for-ieee-to-retire-master-slave  

### Performance considerations 

Compared to a dedicated device implementation on the Raspberry PI, the delegation over the I2C bus to a slower 
microcontroller will lead to a much slower processing of such IOT instructions. The advantage of using I2CB lies
more in the simplicity of developing code that interacts with physical displays, sensors, actuators and other gadgets 
that often have e.g. MicroPython or CircuitPython code snippets available that can be used with microcontrollers 
like the Raspberry Pi Pico or similar products. 

Once you are satisfied with the functionality of a MicroPython or CircuitPython script of your microcontroller, you might want to consider
rewriting the code in a faster language like C/C++. 

Another way to improve performance of your project when using I2CB is to increase the Raspberry Pi's I2C clock rate.
At the time of writing (2021), the default clock rate for the I2C bus on Raspberry Pi OS is set to 100 kHz only.
And on some Rapberry Pi models, the I2C clock can vary with the CPU clock as the CPU load changes, see a dicussion here :

https://forums.raspberrypi.com/viewtopic.php?t=155756#p1112968
    
So if you want to really optimize the thoughput of the I2C bus on the Raspberry Pi, you will probably want to change the I2C clock 
settings and the CPU clock scaling on the Pi. For lower speed applications (say up to only a few 100 IOT transactions per second),
you can probably live with the default settings.
    

### Raspberry Pi Pico specific challenge: Use the Pico as an I2C responder with MicroPython 

At the time of writing (2021), MicroPython for the Rasperry Pi Pico had difficulties using the Pico as the responder (as opposed to the controller) on
the I2C bus. Until this is fixed, you can use this solution to overcome this problem:

https://github.com/epmoyer/I2CResponder


### <a id="license"></a>License

Copyright 2021 by Heinz-Bernd Eggenstein. Licensed under the terms of
[the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
