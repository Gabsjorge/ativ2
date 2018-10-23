obj-m += projeto.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc teste.c -o teste
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean