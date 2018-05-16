obj-m+=hu_device.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) test_program.c -o test_program.out
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test_program.out