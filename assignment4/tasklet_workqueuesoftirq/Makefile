obj-m += demo.o
KCFLAGS += CC=/usr/bin/gcc-12

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
