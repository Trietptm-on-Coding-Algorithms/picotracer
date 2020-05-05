PIN_ROOT=/opt/pin

.PHONY: all clean picotracer

all: picotracer

picotracer: picotracer/makefile picotracer/makefile.rules picotracer/picotracer.cpp
	export PIN_ROOT=$(PIN_ROOT) && cd picotracer && $(MAKE)

clean:
	cd picotracer && rm -rf obj-intel64

