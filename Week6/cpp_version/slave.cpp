#include <string.h>
#include <iostream>
#include <pigpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <cstdint>
#define SLAVE_ADDR 0x00

typedef struct message{
        uint8_t code;
        uint8_t address;
	uint8_t aux1;
	uint8_t aux2;
	float value[4];
} message_t;

int init_slave(bsc_xfer_t &xfer, int addr){

        gpioSetMode(18, PI_ALT3);
        gpioSetMode(19, PI_ALT3);
        xfer.control = (addr<<16) |
                        (0x00<<13) |
                        (0x00<<12) |
                        (0x00<<11) |
                        (0x00<<10) |
                        (0x01<<9) |
                        (0x01<<8) |
                        (0x00<<7) |
                        (0x00<<6) |
                        (0x00<<5) |
                        (0x00<<4) |
                        (0x00<<3) |
                        (0x01<<2) |
                        (0x00<<1) |
                        0x01;
        return bscXfer(&xfer);
}

int main(void){

	std::cout << (int)BSC_FIFO_SIZE << std::endl;
return 0; 
        message_t message;

        bsc_xfer_t xfer;

        if(gpioInitialise() < 0){
                std::cout << "Error 1" << std::endl;
                return 1;
        }

        int status;
        status = init_slave(xfer, SLAVE_ADDR);

        std::cout << "Started program with status " << status << std::endl;

        while(1){
                xfer.txCnt = 0;
                status = bscXfer(&xfer);
                if(status){
                        if(xfer.rxCnt > 0){
                                std::cout << "Received "<< xfer.rxCnt << " bytes" << std::endl;
                                memcpy(&message, xfer.rxBuf, sizeof(message_t));
                                std::cout << "address: " << (int)message.address << std::endl <<
					     "code: " << (int)message.code << std::endl <<
				             "aux1: " << (int)message.aux1 << std::endl <<
                                             "aux2: " << (int)message.aux2 << std::endl <<
                                             "value0: " << message.value[0] << std::endl <<
                                             "value1: " << message.value[1] << std::endl <<
                                             "value2: " << message.value[2] << std::endl <<
                                             "value3: " << message.value[3] << std::endl;
                                memset(xfer.rxBuf, 0 , BSC_FIFO_SIZE );
                                xfer.rxCnt=0;
                        }
                }
        }
}
