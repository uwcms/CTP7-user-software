# Cross-compiling to a Petalinux target
#ifdef PETA

PETADIST=$(PETALINUX)/software/petalinux-dist
PETACTP7MEMSVC=libmemsvc/
CC=$(PETALINUX)/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/arm-xilinx-linux-gnueabi-c++

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
         -static

LIBDIR = ../pl-lib
BINDIR = ../pl-bin
SHLIB=
#else
#CFLAGS = -g -DLINUX -fPIC -I $(VMEDIR)/include -I $(RCTCOREDIR)/include
#LIBDIR = ../lib
#BINDIR = ../bin

#SHLIB=
#LDFLAGS=-L $(LIBDIR) -lVME -lRCTCore -lJCCTest -L /opt/xdaq/lib -lCAENVME -lCTP7Play -L$(PETACTP7MEMSVC) -lmemsvc -llog4cplus 
#CXXFLAGS=-g -Wall -DLINUX -DLOG_LEVEL=0x0 -fPIC -I $(VMEDIR)/include -I $(RCTCOREDIR)/include  

#endif


include ../mkdep_makefile/config


