To install pidp8:
-----------------
Choose the default script, unless you have done the 'serial mod'.
Run under sudo;
Make sure you run the right version! 

Ensure you have a working internet connection.
Run sudo apt-get update before running the script.


Compile before running install script:
--------------------------------------
Before running the script, compile the pidp8 program:
- cd /opt/pidp8/src
- sudo make
- cd scanswitch
- sudo make

IMPORTANT:
If you have done the 'serial mod', 
first edit the Makefile so that -DSERIALSETUP is in the CFLAGS line.

