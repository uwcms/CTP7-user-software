# Cross-compiling to a Petalinux target
# TODO Fix make clean so that it will rebuild the libraries. Currently it seems not to be doing this.

ifdef PETA

PETADIST=$(PETALINUX)/software/petalinux-dist
PETACTP7MEMSVC=../libmemsvc/
CC=/afs/hep.wisc.edu/cms/sw/Xilinx_2014_2/SDK/2014.2/gnu/arm/lin/bin/arm-xilinx-linux-gnueabi-c++

CXX=$(CC)

CFLAGS=-fomit-frame-pointer -pipe -fno-common -fno-builtin \
       -Wall -Werror \
       -march=armv7-a -mfpu=neon -mfloat-abi=softfp \
       -mthumb-interwork -mtune=cortex-a9 \
       -DEMBED -Dlinux -D__linux__ -Dunix -DLINUX -fPIC \
       -I$(PETADIST)/stage/include \
       -I$(PETADIST)/stage/usr/include \
       -I$(PETADIST)/stage/usr/local/include \
       -I$(PETADIST) -I$(PETADIST)/include \
       -I$(PETACTP7MEMSVC) \
       -I$(RCTCOREDIR)/include 

LDFLAGS= -L$(PETADIST)/lib -L$(PETADIST)/stage/lib \
         -L$(PETADIST)/stage/usr/lib -L$(PETADIST)/stage/usr/local/lib \
         -L$(PETACTP7MEMSVC) -lmemsvc \
	 -lpthread \
         -static

LIBDIR = ../../pl-lib
BINDIR = ../../pl-bin
SHLIB=

else

CFLAGS = -g 
LIBDIR = ../../lib
BINDIR = ../../bin
SHLIB =
LDFLAGS = -L $(LIBDIR) -lCTP7Server
CXXFLAGS = $(CFLAGS)
endif


include ../../mkdep_makefile/config


