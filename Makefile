obj-m += nes.o
hver = 4.19.66-v7+
LDLIBS = -lwiringPi
all: 
	make -C /lib/modules/$(hver)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(hver)/build M=$(PWD) clean
