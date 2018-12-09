#include <iostream>
#include <thread>
#include <chrono>
//#include <boost/asio.hpp>

int main(void){
	char command, argument;
	double value;

	for(;;){
		if(std::cin >> command){
			std::cout << "Command received." << std::endl;

			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if(std::cin >> argument){
				std::cout << "Argument to command " << command << " received." << std::endl;

				if(std::cin >> value){
					std::cout << "Full command " << command << ' ' << argument << ' ' << value << " received." << std::endl;
				}
				else{
					std::cout << "Invalid value for provided command " << command << ' ' << argument << '.' << std::endl;
				}
			}
		}
		std::this_thread::sleep_for (std::chrono::seconds(1));
	}

	return 0;
}
