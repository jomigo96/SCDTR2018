#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Server.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cstring>
#include "data_structures.hpp"
#include <pigpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <deque>
#define SLAVE_ADDR 0x00

//#define DEBUG

std::mutex m;
std::condition_variable cv;
bool ready = false;
const int desk_count = 2;
desk_t desks[desk_count];
std::pair< std::deque<std::pair<float, float>> , std::deque<std::pair<float, float> > > last_minute_buffer;


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

void data_manager_thread(){

	bsc_xfer_t xfer;
	message_t message;
	const float lux_high = 250;
	const float h = 0.005;
	uint32_t timestamp;

	if(gpioInitialise() < 0){
                std::cerr << "Error initializing gpio" << std::endl;
                return;
    	}

	int status = init_slave(xfer, 0);

	std::cout << "Started with status " << status << std::endl;

	timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m.lock();
	memset(desks, 0, 2*sizeof(desk_t));
	desks[0].time_acc = timestamp;
	desks[1].time_acc = timestamp;
	m.unlock();

	while(1){
		xfer.txCnt = 0;
		status=bscXfer(&xfer);
		if(status && (xfer.rxCnt > 0)){
			memcpy(&message, xfer.rxBuf, sizeof(message_t));
			memset(xfer.rxBuf, 0 , BSC_FIFO_SIZE );
#ifdef DEBUG
            std::cout << "Data with size " << xfer.rxCnt << " from "<< (int) message.address << std::endl;
#endif
            xfer.rxCnt = 0;

            if((message.address != 1) && (message.address != 9))
                continue; // Data is corruputed
            int desk = (message.address == 1) ? 0 : 1;

			if(message.code == sampling_time_data){

				m.lock();
				desks[desk].samples++;
				desks[desk].l_pprev = desks[desk].l_prev;
				desks[desk].l_prev = desks[desk].illuminance;
				desks[desk].illuminance = message.value[0];
				desks[desk].power = message.value[1];
				desks[desk].illuminance_ref = message.value[2];
				desks[desk].duty_cycle = message.aux1;
				desks[desk].illuminance_lb = message.aux2;
				desks[desk].occupancy = (desks[desk].illuminance_lb == lux_high) ? 1 : 0;
				desks[desk].energy_acc += (desks[desk].power * h);
				if(desks[desk].illuminance < desks[desk].illuminance_lb){
					desks[desk].comfort_error_acc += (desks[desk].illuminance_lb-desks[desk].illuminance);
					desks[desk].comf_err_samples++;
				}
				if(desks[desk].samples >= 3){
					if((desks[desk].l_prev-desks[desk].l_pprev)*(desks[desk].illuminance-desks[desk].l_prev)<0){
						desks[desk].comfort_flicker_acc += (fabs(desks[desk].l_prev-desks[desk].l_pprev)+fabs(desks[desk].illuminance-desks[desk].l_prev))/(2*h);
					}
				}

                if(desk == 0){
                    last_minute_buffer.first.push_back(std::pair<desks[0].illuminance, desks[0].duty_cycle>);
                    if(last_minute_buffer.first.size()>20)
                        last_minute_buffer.first.pop_front();
                }else{
                    last_minute_buffer.second.push_back(std::pair<desks[1].illuminance, desks[1].duty_cycle>);
                    if(last_minute_buffer.second.size()>20)
                        last_minute_buffer.second.pop_front();
                }

				m.unlock();
			}else if(message.code == data){ //Calibration finished, counts as a restart
				timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				m.lock();
				memset(&desks[desk], 0, sizeof(desk_t));
				desks[desk].time_acc = timestamp;
                desks[desk].illuminance_bg = message.value[0];
				m.unlock();
			}
			xfer.rxCnt=0;
		}
	}
}


int main(){

	std::thread th(data_manager_thread);

	try{
		boost::asio::io_service io;
		Server ser(io);
		ser.start_sever(8080);
		io.run();

	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}

	th.join();
	return 0;
}
