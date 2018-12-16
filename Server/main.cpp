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
