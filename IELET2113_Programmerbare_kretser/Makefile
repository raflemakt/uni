# Simple VHDL makefile for fast testing of designs.
#
# Usage:
# 	- edit SRCFILES to include all .vhd files in the project
# 	- edit TOP to the name of your top level entity
# 	- increase SIM_TIME if needed
#
# 	- make			<- analyse, elaborate and execute design
# 	- make wave		<- open outputted waveform in gtkwave
# 	- make clean	<- remove 'work'
# 
# Dependencies:
# 	- nvc (VHDL compiler and simulator)
# 	- gtkwave (to view waveforms, optional)

SIM=nvc
FLAGS=--std=2008
SRCFILES=file.vhd
TOP=top_level_entity
SIM_TIME=20us

all:
	$(SIM) $(FLAGS) -a $(SRCFILES)
	$(SIM) $(FLAGS) -e $(TOP)
	$(SIM) $(FLAGS) -r $(TOP) --wave=work/wave.ghw --stop-time=$(SIM_TIME)

wave:
	gtkwave work/wave.ghw &

clean:
	rm -r work/*
	rmdir work
