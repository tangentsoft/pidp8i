# CC=gcc

CFLAGS=-std=c99 -U__STRICT_ANSI__  -Wno-unused-result -D_GNU_SOURCE -DUSE_READER_THREAD -DHAVE_DLOPEN=so -I . -I PDP8 -DPIDP8

# uncomment below for 'serial mod'
#CFLAGS+=-DSERIALSETUP

DEPS = gpio.h sim_console.h sim_ether.h sim_rev.h sim_tape.h sim_tmxr.h scp.h sim_defs.h sim_fio.h sim_sock.h sim_timer.h PDP8/pdp8_defs.h

OBJ = PDP8/pdp8_cpu.o PDP8/pdp8_clk.o PDP8/pdp8_df.o PDP8/pdp8_dt.o PDP8/pdp8_lp.o PDP8/pdp8_mt.o PDP8/pdp8_pt.o PDP8/pdp8_rf.o PDP8/pdp8_rk.o PDP8/pdp8_rx.o PDP8/pdp8_sys.o PDP8/pdp8_tt.o PDP8/pdp8_ttx.o PDP8/pdp8_rl.o PDP8/pdp8_tsc.o PDP8/pdp8_td.o PDP8/pdp8_ct.o PDP8/pdp8_fpp.o scp.o sim_console.o sim_fio.o sim_timer.o sim_sock.o sim_tmxr.o sim_ether.o sim_tape.o sim_serial.o sim_disk.o gpio.o 

LIBS =  -lm -ldl -lpthread -lrt

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pidp8: $(OBJ)
	mkdir -p ../bin
	$(CC) -o ../bin/$@ $^ $(CFLAGS) $(LIBS)
	sudo setcap 'cap_sys_nice=eip' ../bin/$@

# next two lines force make to always recompile gpio.c
# reason: needed when switching between serial and default version
gpio.o: FORCE
FORCE:

clean:
	rm -f *.o PDP8/*.o
