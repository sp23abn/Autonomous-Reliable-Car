#
# Simple Makefile for compiling programs for initio robot car 
#
SHELL	= bash
GCC	= gcc
CFLAGS	= -Wall -Werror
LFLAGS	= -linitio -lcurses -lwiringPi -lpthread

PROG 	= testIR

.PHONY: all run help

all: $(PROG)

run: $(PROG)
	./$<

schedule: $(PROG)
	rtcs_schedule $(PROG)
	echo "program \"$PROG\" has been scheduled for autostart (within 5 minutes)."

% : %.c
	$(GCC) -o $@ $(CFLAGS) $(LFLAGS) $<

clean:
	rm -f $(PROG)

help:
	@echo
	@echo "Possible commands:"
	@echo " > make run"
	@echo " > make schedule"
	@echo " > make clean"
	@echo

