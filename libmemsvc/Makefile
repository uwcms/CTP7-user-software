ifndef PETALINUX
$(error "Error: PETALINUX environment variable not set.  Change to the root of your PetaLinux install, and source the settings.sh file")
endif

include libs.common.mk

LIB=libmemsvc

# Add any other object files to this list below
LIB_OBJS = libmemsvc.o
LIB_HEADERS=libmemsvc.h

all: build install

.PHONY: build
build: $(LIB).a $(LIB).so
	# Install libraries
	mkdir -p $(STAGEDIR)/usr/lib
	# Specifically do NOT install .a files.
	# We want this dynamically linked only, so we can control changes to it fully.
	#rsync -a *.a $(STAGEDIR)/usr/lib/
	rsync -a *.so $(STAGEDIR)/usr/lib/
	rsync -a  *.so.* $(STAGEDIR)/usr/lib/
	
	# Install headers
	mkdir -p $(STAGEDIR)/usr/include
	for h in $(LIB_HEADERS); do \
		rsync -a $$h $(STAGEDIR)/usr/include; \
	done

$(LIB).a:$(LIB_OBJS)
	$(AR) rcs $@ $(LIB_OBJS)

$(LIB).so:$(LIB_OBJS)
	$(CC) -shared -o $@.1.0.1 $(LIB_OBJS)
	rm -f $@.1 $@
	ln -s $@.1.0.1 $@.1
	ln -s $@.1.0.1 $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $< -std=gnu99
	
install: $(LIB).so
	# Install libraries template
	# Install lib files
	$(TARGETINST) -p 0755 -d $<.1.0.1 /lib/$<.1.0.1
	# Install lib symlink
	$(TARGETINST) -s $<.1.0.1 /lib/$<.1
	$(TARGETINST) -s $<.1.0.1 /lib/$<

clean:
	rm -rf *.o *.so *.so.* *.a

