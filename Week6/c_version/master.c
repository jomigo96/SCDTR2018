#include <stdio.h>
#include <pigpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <memory.h>
#define DESTINATION_ADDR 0x48

int main(int argc, char *argv[]){
	int key = 0;
	int handle;
	int length = 12; //11 chars + \0
	char message[] = "Hello World";
	
	if (gpioInitialise() < 0){
		printf("Erro 1\n");
		return 1;
	}
	
	handle = i2cOpen(1, DESTINATION_ADDR, 0); /* Initialize Master*/
	
	while(key != 'q'){
		i2cWriteDevice(handle, message, length); /* Master Transmit */
		gpioDelay(20000);
		printf("Press q to quit. Any other ckey to continue.\n");
		key = getchar();
	}
	
	i2cClose(handle); /* close master */
	gpioTerminate();
}
