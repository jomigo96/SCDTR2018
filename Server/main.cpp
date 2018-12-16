#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Server.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cstring>
#include "data_structures.hpp"

std::mutex m;
std::condition_variable cv;
bool ready = false;
const int desk_count = 2;
desk_t desks[desk_count];

void data_manager_thread(){



	m.lock();
		desks[0].illuminance=80;
		desks[0].duty_cycle=95;
		desks[0].occupancy=1;
		desks[0].illuminance_lb=80;
		desks[0].illuminance_bg=5;
		desks[0].illuminance_ref=80;
		desks[0].power=0.5;
		desks[0].time_acc=1231;
		desks[0].energy_acc=12.154;
		desks[0].comfort_error_acc=1.54546;
		desks[0].comfort_flicker_acc=5.12121;
		desks[1].illuminance=85;
		desks[1].duty_cycle=50;
		desks[1].occupancy=0;
		desks[1].illuminance_lb=50;
		desks[1].illuminance_bg=10;
		desks[1].illuminance_ref=80;
		desks[1].power=0.4;
		desks[1].time_acc=12321;
		desks[1].energy_acc=12;
		desks[1].comfort_error_acc=1.54;
		desks[1].comfort_flicker_acc=1.02;
	m.unlock();

	while(1){
		std::unique_lock<std::mutex> lck(m);
		while(!ready)
			cv.wait(lck);

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
