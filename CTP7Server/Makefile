# Cross-compiling to a Petalinux target
# TODO Fix make clean so that it will rebuild the libraries. Currently it seems not to be doing this.

LIBDIR = .
BINDIR = .
SHLIB=

ifdef PETA

PETA_ROOTFS=/afs/hep.wisc.edu/home/uwhepfpga/petalinux-v2013.10-final/CTP7/build/linux/rootfs

PETACTP7MEMSVC=../libmemsvc/
CC=/afs/hep.wisc.edu/cms/sw/Xilinx_2014_2/SDK/2014.2/gnu/arm/lin/bin/arm-xilinx-linux-gnueabi-c++

CXX=$(CC)

CFLAGS=-fomit-frame-pointer -pipe -fno-common -fno-builtin \
       -Wall \
       -march=armv7-a -mfpu=neon -mfloat-abi=softfp \
       -mthumb-interwork -mtune=cortex-a9 \
       -DEMBED -Dlinux -D__linux__ -Dunix -DLINUX -fPIC \
       -I$(PETA_ROOTFS)/stage/include \
       -I$(PETA_ROOTFS)/stage/usr/include \
       -I$(PETA_ROOTFS)/stage/usr/local/include \
       -I$(PETA_ROOTFS)/libs/libmemsvc

LDFLAGS=-L$(PETA_ROOTFS)/targetroot/lib \
	-L$(PETA_ROOTFS)/stage/lib \
	-L$(PETA_ROOTFS)/targetroot/usr/lib \
	-L$(PETA_ROOTFS)/stage/usr/lib \
        -lmemsvc -lpthread

BUILDINFO_PATH=../../apps/buildinfo


else

CFLAGS = -g 
LDFLAGS = -L $(LIBDIR) -lCTP7Server -lpthread
CXXFLAGS = $(CFLAGS)

endif

include ../../mkdep_makefile/config
